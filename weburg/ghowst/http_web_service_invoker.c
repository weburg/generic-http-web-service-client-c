#include <stdlib.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <curl/curl.h>
#include <glib.h>

#include "http_web_service_invoker.h"

typedef struct ghowst_handle {
    char *base_url;
    CURL *curl_handle;
    ghowsthttp_web_service_error_t last_http_web_service_error;
} ghowst_handle_t;

// Begin cURL dependencies

struct memory {
    char *memory;
    size_t size;
};

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct memory *mem = userp;
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if (mem->memory == NULL) {
        /* out of memory! */
        printf("not enough memory (realloc returned NULL)\n");
        return 0;
    }

    memcpy(&(mem->memory[mem->size]), contents, realsize);
    mem->size += realsize;
    mem->memory[mem->size] = 0;

    return realsize;
}

// End cURL dependencies

static char *get_entity_name(const char *name, const char *verb)
{
    size_t prefix_length = strlen(verb);
    if (strlen(name) <= prefix_length || name[prefix_length] != '_') {
        return NULL;
    }

    char *lower = g_ascii_strdown(name, -1);

    memmove(lower, lower + strlen(verb) + 1, strlen(lower + strlen(verb) + 1) + 1);

    return lower;
}

static _Bool starts_with(const char *string, const char *prefix)
{
    while (*prefix) {
        if (*prefix++ != *string++) {
            return false;
        }
    }

    return true;
}

static char *underbar_to_camel(char *dest, const char *str)
{
    char *dest_copy = dest;

    _Bool uppercase_next = false;

    while (*str != '\0') {
        if (uppercase_next) {
            *dest_copy = (char) toupper(*str);
            uppercase_next = false;
        } else if (*str != '_') {
            *dest_copy = (char) *str;
        }

        if (*str == '_') {
            uppercase_next = true;
        } else {
            dest_copy++;
        }

        str++;
    }

    *dest_copy = '\0';

    return dest;
}

static char *generate_form_data(ghowst_url_parameter_t *arguments, int num_args)
{
    CURL *curl_handle = curl_easy_init();

    GString *qs = g_string_new("");

    for (int i = 0; i < num_args; i++, arguments++) {
        char *escaped_value = curl_easy_escape(curl_handle, arguments->value, strlen(arguments->value));

        if (i > 0) {
            g_string_append(qs, "&");
        }

        char *new_name = calloc(strlen(arguments->name) + 1, sizeof *new_name);
        g_string_append(qs, underbar_to_camel(new_name, arguments->name));
        g_string_append(qs, "=");
        g_string_append(qs, escaped_value);
        free(new_name);

        curl_free(escaped_value);
    }

    curl_easy_cleanup(curl_handle);

    return g_string_free(qs, FALSE);
}

static char *generate_qs(ghowst_url_parameter_t *arguments, int num_args)
{
    GString *query_string = g_string_new((num_args > 0 ? "?" : ""));

    if (num_args > 0) {
        char *form_data = generate_form_data(arguments, num_args);

        g_string_append(query_string, form_data);

        g_free(form_data);
    }

    return g_string_free(query_string, FALSE);
}

static void check_error(CURLcode result_code, ghowst_handle_t *ghowst)
{
    if (result_code == CURLE_OK) {
        long http_status_code;
        curl_easy_getinfo(ghowst->curl_handle, CURLINFO_RESPONSE_CODE, &http_status_code);

        struct curl_header *header;
        CURLHcode h;

        if (http_status_code >= 400 || http_status_code < 200) {
            ghowst->last_http_web_service_error.error = true;
            h = curl_easy_header(ghowst->curl_handle, "x-error-message", 0, CURLH_HEADER, -1, &header);
            if (h == CURLHE_OK) {
                ghowst->last_http_web_service_error.message = header->value;
            }
        } else if (http_status_code >= 300 && http_status_code < 400) {
            ghowst->last_http_web_service_error.error = true;
            h = curl_easy_header(ghowst->curl_handle, "location", 0, CURLH_HEADER, -1, &header);
            if (h == CURLHE_OK) {
                ghowst->last_http_web_service_error.message = header->value;
            }
        } else {
            ghowst->last_http_web_service_error.error = false;
            ghowst->last_http_web_service_error.message = "";
        }

        ghowst->last_http_web_service_error.http_status = http_status_code;
    } else {
        ghowst->last_http_web_service_error.error = true;
        ghowst->last_http_web_service_error.http_status = 0;
        ghowst->last_http_web_service_error.message = curl_easy_strerror(result_code);
    }
}

static void execute_and_handle(ghowst_handle_t *ghowst, struct memory *chunk, struct curl_slist *headers, ghowst_url_parameter_t *arguments, int num_args, _Bool has_file, curl_mime *mime_handle)
{
    curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
    curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEDATA, (void *) chunk);
    curl_easy_setopt(ghowst->curl_handle, CURLOPT_HTTPHEADER, headers);

    CURLcode result_code = curl_easy_perform(ghowst->curl_handle);

    check_error(result_code, ghowst);

    if (has_file) {
        curl_mime_free(mime_handle);

        ghowst_url_parameter_t *arguments_copy = arguments;
        for (int i = 0; i < num_args; i++, arguments_copy++) {
            if (arguments_copy->file) {
                fclose(arguments_copy->file);
            }
        }
    }
}

static _Bool write_http_entity_from_arguments(ghowst_handle_t *ghowst, ghowst_url_parameter_t *arguments, int num_args, char *query_string, curl_mime *mime_handle)
{
    _Bool has_file = false;
    ghowst_url_parameter_t *arguments_copy;

    arguments_copy = arguments;
    for (int i = 0; i < num_args; i++, arguments_copy++) {
        if (arguments_copy->file != NULL) {
            has_file = true;
            break;
        }
    }

    if (!has_file) {
        query_string = generate_form_data(arguments, num_args);

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_POSTFIELDS, query_string);
    } else {
        mime_handle = curl_mime_init(ghowst->curl_handle);
        curl_mimepart *mime_part;

        arguments_copy = arguments;
        for (int i = 0; i < num_args; i++, arguments_copy++) {
            if (arguments_copy->file == NULL) {
                char *new_name = calloc(strlen(arguments_copy->name) + 1, sizeof *new_name);
                mime_part = curl_mime_addpart(mime_handle);
                curl_mime_name(mime_part, underbar_to_camel(new_name, arguments_copy->name));
                curl_mime_data(mime_part, arguments_copy->value, CURL_ZERO_TERMINATED);
                free(new_name);
            } else if (arguments_copy->file) {
                fseek(arguments_copy->file, 0, SEEK_END);
                long length = ftell(arguments_copy->file);
                fseek(arguments_copy->file, 0, SEEK_SET);

                char *new_name = calloc(strlen(arguments_copy->name) + 1, sizeof *new_name);
                mime_part = curl_mime_addpart(mime_handle);
                curl_mime_name(mime_part, underbar_to_camel(new_name, arguments_copy->name));
                curl_mime_filename(mime_part, arguments_copy->file_name);
                curl_mime_data_cb(mime_part, length, (curl_read_callback) fread,
                                  (curl_seek_callback) fseek, NULL, arguments_copy->file);
                free(new_name);
            }
        }

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_MIMEPOST, mime_handle);
    }

    return has_file;
}

char *ghowst_invoke(ghowsth ghowsth, const char *method_name, ghowst_url_parameter_t *arguments, int num_args)
{
    ghowst_handle_t *ghowst = (ghowst_handle_t *) ghowsth;

    char *verb = NULL;
    char *resource = NULL;

    if (starts_with(method_name, "get")) {
        verb = g_strdup("get");
        resource = get_entity_name(method_name, verb);
    } else if (starts_with(method_name, "create_or_replace")) {
        verb = g_strdup("create_or_replace");
        resource = get_entity_name(method_name, verb);
    } else if (starts_with(method_name, "create")) {
        verb = g_strdup("create");
        resource = get_entity_name(method_name, verb);
    } else if (starts_with(method_name, "update") ) {
        verb = g_strdup("update");
        resource = get_entity_name(method_name, verb);
    } else if (starts_with(method_name, "delete")) {
        verb = g_strdup("delete");
        resource = get_entity_name(method_name, verb);
    } else {
        gchar **method_parts = g_strsplit(method_name, "_", 2);

        if (method_parts[0] != NULL) {
            verb = g_strdup(method_parts[0]);
            resource = get_entity_name(method_name, verb);
        }

        g_strfreev(method_parts);
    }

    fprintf(stderr, "Verb: %s\n", verb);
    fprintf(stderr,"Resource: %s\n", resource);

    ghowst->curl_handle = curl_easy_init();
    curl_mime *mime_handle = curl_mime_init(ghowst->curl_handle);
    struct curl_slist *headers = curl_slist_append(NULL, "accept: application/json");

    struct memory chunk;
    chunk.memory = malloc(1); // Will be grown as needed
    chunk.size = 0; // No data at this point

    if (strcmp(verb, "get") == 0) {
        char *query_string = generate_qs(arguments, num_args);
        GString *ws_url = g_string_new(ghowst->base_url);
        g_string_append(ws_url, "/");
        g_string_append(ws_url, resource);
        g_string_append(ws_url, query_string);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url->str);

        execute_and_handle(ghowst, &chunk, headers, arguments, num_args, false, mime_handle);
        g_string_free(ws_url, TRUE);
        g_free(query_string);
    } else if (strcmp(verb, "create") == 0 || strcmp(verb, "create_or_replace") == 0 || strcmp(verb, "update") == 0) {
        if (strcmp(verb, "create") == 0) {
            curl_easy_setopt(ghowst->curl_handle, CURLOPT_POST, true);
        } else if (strcmp(verb, "create_or_replace") == 0) {
            curl_easy_setopt(ghowst->curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
        } else if (strcmp(verb, "update") == 0) {
            curl_easy_setopt(ghowst->curl_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
        }

        char *query_string = NULL;
        _Bool has_file = write_http_entity_from_arguments(ghowst, arguments, num_args, query_string, mime_handle);

        GString *ws_url = g_string_new(ghowst->base_url);
        g_string_append(ws_url, "/");
        g_string_append(ws_url, resource);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url->str);

        execute_and_handle(ghowst, &chunk, headers, arguments, num_args, has_file, mime_handle);
        g_string_free(ws_url, TRUE);
    } else if (strcmp(verb, "delete") == 0) {
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");

        char *query_string = generate_qs(arguments, num_args);
        GString *ws_url = g_string_new(ghowst->base_url);
        g_string_append(ws_url, "/");
        g_string_append(ws_url, resource);
        g_string_append(ws_url, query_string);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url->str);

        execute_and_handle(ghowst, &chunk, headers, arguments, num_args, false, mime_handle);
        g_string_free(ws_url, TRUE);
        g_free(query_string);
    } else {
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_POST, true);

        char *query_string = NULL;
        _Bool has_file = write_http_entity_from_arguments(ghowst, arguments, num_args, query_string, mime_handle);

        GString *ws_url = g_string_new(ghowst->base_url);
        g_string_append(ws_url, "/");
        g_string_append(ws_url, resource);
        g_string_append(ws_url, "/");
        g_string_append(ws_url, verb);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url->str);

        execute_and_handle(ghowst, &chunk, headers, arguments, num_args, has_file, mime_handle);
        g_string_free(ws_url, TRUE);
    }

    curl_slist_free_all(headers);
    g_free(verb);
    g_free(resource);
    return chunk.memory;
}

ghowsthttp_web_service_error_t ghowst_last_error(ghowsth ghowsth)
{
    ghowst_handle_t *ghowst = (ghowst_handle_t *) ghowsth;

    return ghowst->last_http_web_service_error;
}

ghowsth ghowst_init(char *base_url)
{
    ghowst_handle_t *ghowst = malloc(sizeof *ghowst);

    curl_global_init(CURL_GLOBAL_ALL);

    ghowst->base_url = base_url;

    return (ghowsth) ghowst;
}

void ghowst_cleanup(ghowsth ghowsth)
{
    ghowst_handle_t *ghowst = (ghowst_handle_t *) ghowsth;

    curl_easy_cleanup(ghowst->curl_handle);
    curl_global_cleanup();

    free(ghowst);
}