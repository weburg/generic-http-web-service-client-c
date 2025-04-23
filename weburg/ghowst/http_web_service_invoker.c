#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>

#include "http_web_service_invoker.h"

typedef struct ghowsthandle {
    char *base_url;
    CURL *curl_handle;
    ghowsthttp_web_service_error last_http_web_service_error;
} ghowsthandle;

// Begin cURL dependencies

struct memory_struct {
    char *memory;
    size_t size;
};

static size_t write_memory_callback(void *contents, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct memory_struct *mem = (struct memory_struct *) userp;
    mem->memory = realloc(mem->memory, mem->size + realsize + 1);
    if(mem->memory == NULL) {
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

static char *weburg_strlwr(char *dest, const char *str)
{
    char *dest_copy = dest;

    while (*str != '\0') {
        *dest_copy = (char) tolower(*str);
        str++;
        dest_copy++;
    }

    *dest_copy = '\0';

    return dest;
}

static char *get_entity_name(const char *name, const char *verb)
{
    char lower_name[128];
    char *lower_name_ptr = weburg_strlwr(lower_name, name);

    lower_name_ptr += strlen(verb) + 1; // Extra to get rid of trailing underscore

    return lower_name_ptr;
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

static char *generate_form_data(ghowst_url_parameter *arguments, int num_args)
{
    CURL *curl_handle = curl_easy_init();

    char *qs = calloc((128 * num_args) + 1, sizeof *qs);

    for (int i = 0; i < num_args; i++, arguments++) {
        char *escaped_value = curl_easy_escape(curl_handle, arguments->value, strlen(arguments->value));

        if (i > 0) {
            strncat(qs, "&", 1);
        }

        char *new_name = calloc(strlen(arguments->name) + 1, sizeof *new_name);
        strncat(qs, underbar_to_camel(new_name, arguments->name), strlen(arguments->name));
        strncat(qs, "=", 1);
        strncat(qs, escaped_value, strlen(escaped_value));
        free(new_name);

        curl_free(escaped_value);
    }

    curl_easy_cleanup(curl_handle);

    return qs;
}

static char *generate_qs(ghowst_url_parameter *arguments, int num_args)
{
    char *qs = calloc(99, sizeof *qs);

    if (num_args > 0) {
        strncat(qs, "?", 1);

        char *form_data = generate_form_data(arguments, num_args);

        strncat(qs, form_data, strlen(form_data));

        free(form_data);
    }

    return qs;
}

static char *generate_mimeqs()
{
    // TODO move the file upload field generation code here to support file
    // uploads in other calls.

    char *qs = {"\0"};

    return qs;
}

static void check_error(CURLcode result_code, ghowsthandle *ghowst)
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

char *ghowst_invoke(ghowsth *ghowsth, const char *method_name, ghowst_url_parameter *arguments, int num_args)
{
    ghowsthandle *ghowst = (ghowsthandle *) ghowsth;

    char verb[20];
    char entity[128];

    if (starts_with(method_name, "get")) {
        strncpy(verb, "get", 20);
        strncpy(entity, get_entity_name(method_name, verb), 128);
    } else if (starts_with(method_name, "create_or_replace")) {
        strncpy(verb, "create_or_replace", 20);
        strncpy(entity, get_entity_name(method_name, verb), 128);
    } else if (starts_with(method_name, "create")) {
        strncpy(verb, "create", 20);
        strncpy(entity, get_entity_name(method_name, verb), 128);
    } else if (starts_with(method_name, "update") ) {
        strncpy(verb, "update", 20);
        strncpy(entity, get_entity_name(method_name, verb), 128);
    } else if (starts_with(method_name, "delete")) {
        strncpy(verb, "delete", 20);
        strncpy(entity, get_entity_name(method_name, verb), 128);
    } else {
        char *method_name_to_scan = strdup(method_name);
        char *tokenized_string = strtok(method_name_to_scan, "_");

        strncpy(verb, tokenized_string, 20);
        strncpy(entity, get_entity_name(method_name, verb), 128);
    }

    printf("Verb: %s\n", verb);
    printf("Entity: %s\n", entity);

    CURLcode result_code;
    ghowst->curl_handle = curl_easy_init();
    struct curl_slist *headers = curl_slist_append(NULL, "accept: application/json");

    struct memory_struct chunk;
    chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
    chunk.size = 0; /* no data at this point */

    if (strcmp(verb, "get") == 0) {
        char *query_string = generate_qs(arguments, num_args);

        char ws_url[300];
        strncpy(ws_url, ghowst->base_url, 150);
        strncat(ws_url, "/", 1);
        strncat(ws_url, entity, 50);
        strncat(ws_url, query_string, strlen(query_string));

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url);

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_HTTPHEADER, headers);

        result_code = curl_easy_perform(ghowst->curl_handle);

        free(query_string);

        check_error(result_code, ghowst);
    } else if (strcmp(verb, "create") == 0) {
        _Bool has_file = false;
        ghowst_url_parameter *arguments_copy;

        arguments_copy = arguments;
        for (int i = 0; i < num_args; i++, arguments_copy++) {
            if (arguments_copy->file != NULL) {
                has_file = true;
                break;
            }
        }

        char ws_url[300];
        strncpy(ws_url, ghowst->base_url, 150);
        strncat(ws_url, "/", 1);
        strncat(ws_url, entity, 25);

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url);

        curl_mime *mime_handle;

        char *query_string = NULL;

        if (!has_file) {
            query_string = generate_form_data(arguments, num_args);

            curl_easy_setopt(ghowst->curl_handle, CURLOPT_POST, true);
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

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_HTTPHEADER, headers);

        result_code = curl_easy_perform(ghowst->curl_handle);

        if (query_string != NULL) {
            free(query_string);
        }

        check_error(result_code, ghowst);

        if (has_file) {
            curl_mime_free(mime_handle);

            arguments_copy = arguments;
            for (int i = 0; i < num_args; i++, arguments_copy++) {
                if (arguments_copy->file) {
                    fclose(arguments_copy->file);
                }
            }
        }
    } else if (strcmp(verb, "create_or_replace") == 0) {
        char *query_string = generate_form_data(arguments, num_args);

        char ws_url[300];
        strncpy(ws_url, ghowst->base_url, 150);
        strncat(ws_url, "/", 1);
        strncat(ws_url, entity, 50);

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_POSTFIELDS, query_string);

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_HTTPHEADER, headers);

        result_code = curl_easy_perform(ghowst->curl_handle);

        free(query_string);

        check_error(result_code, ghowst);
    } else if (strcmp(verb, "update") == 0) {
        char *query_string = generate_form_data(arguments, num_args);

        char ws_url[300];
        strncpy(ws_url, ghowst->base_url, 150);
        strncat(ws_url, "/", 1);
        strncat(ws_url, entity, 50);

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_POSTFIELDS, query_string);

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_HTTPHEADER, headers);

        result_code = curl_easy_perform(ghowst->curl_handle);

        free(query_string);

        check_error(result_code, ghowst);
    } else if (strcmp(verb, "delete") == 0) {
        char *query_string = generate_qs(arguments, num_args);

        char ws_url[300];
        strncpy(ws_url, ghowst->base_url, 150);
        strncat(ws_url, "/", 1);
        strncat(ws_url, entity, 50);
        strncat(ws_url, query_string, strlen(query_string));

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_HTTPHEADER, headers);

        result_code = curl_easy_perform(ghowst->curl_handle);

        free(query_string);

        check_error(result_code, ghowst);
    } else {
        char *query_string = generate_form_data(arguments, num_args);

        char ws_url[300];
        strncpy(ws_url, ghowst->base_url, 150);
        strncat(ws_url, "/", 1);
        strncat(ws_url, entity, 50);
        strncat(ws_url, "/", 1);
        strncat(ws_url, verb, 50);

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_URL, ws_url);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_POST, true);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_POSTFIELDS, query_string);

        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
        curl_easy_setopt(ghowst->curl_handle, CURLOPT_HTTPHEADER, headers);

        result_code = curl_easy_perform(ghowst->curl_handle);

        free(query_string);

        check_error(result_code, ghowst);
    }

    curl_slist_free_all(headers);
    return chunk.memory;
}

ghowsthttp_web_service_error ghowst_last_error(ghowsth *ghowsth)
{
    ghowsthandle *ghowst = (ghowsthandle *) ghowsth;

    return ghowst->last_http_web_service_error;
}

ghowsth *ghowst_init(char *base_url)
{
    ghowsthandle *ghowst = malloc(sizeof *ghowst);

    curl_global_init(CURL_GLOBAL_ALL);

    ghowst->base_url = base_url;

    return ghowst;
}

void ghowst_cleanup(ghowsth *ghowsth)
{
    ghowsthandle *ghowst = (ghowsthandle *) ghowsth;

    curl_easy_cleanup(ghowst->curl_handle);
    curl_global_cleanup();

    free(ghowst);
}