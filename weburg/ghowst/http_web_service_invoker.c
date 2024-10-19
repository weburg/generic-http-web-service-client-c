#include <ctype.h>
#include "curl/curl.h"
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "http_web_service_invoker.h"

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

char *invoke(const char *method_name, struct url_parameter *arguments, int num_args, char *base_url)
{
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
	struct memory_struct chunk;
	chunk.memory = malloc(1); /* will be grown as needed by the realloc above */
	chunk.size = 0; /* no data at this point */
	curl_global_init(CURL_GLOBAL_ALL);
	CURL *curl_handle = curl_easy_init();

	if (strcmp(verb, "get") == 0) {
		char id_urlsafe[20] = {0};

		for (int i = 0; i < num_args; i++, arguments++) {
			if (strcmp(arguments->name, "id") == 0 && strcmp(arguments->value, "0") != 0) {
				char *id_o = curl_easy_escape(curl_handle, arguments->value, 0);
				strncpy(id_urlsafe, id_o, 20);
				curl_free(id_o);
			}
		}

		char ws_url[300];
		strncpy(ws_url, base_url, 150);
		strncat(ws_url, "/", 1);
		strncat(ws_url, entity, 50);

		if (strlen(id_urlsafe) > 0) {
			strncat(ws_url, "?id=", 4);
			strncat(ws_url, id_urlsafe, 20);
		}

		curl_easy_setopt(curl_handle, CURLOPT_URL, ws_url);

		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
		result_code = curl_easy_perform(curl_handle);

		if (result_code == CURLE_OK) { // TODO better error handling
			//printf("The cURL request was good: %s\n", chunk.memory);
		} else {
			printf("The cURL request went to heck\n");
		}
	} else if (strcmp(verb, "create") == 0) {
		char query_string[512] = {0};

		_Bool has_file = false;
		struct url_parameter *arguments_copy;

		arguments_copy = arguments;
		for (int i = 0; i < num_args; i++, arguments_copy++) {
			if (arguments_copy->file != NULL) {
				has_file = true;
				break;
			}
		}

		char ws_url[300];
		strncpy(ws_url, base_url, 150);
		strncat(ws_url, "/", 1);
		strncat(ws_url, entity, 25);

		curl_easy_setopt(curl_handle, CURLOPT_URL, ws_url);

		curl_mime *mime_handle;

		if (!has_file) {
			arguments_copy = arguments;
			for (int i = 0, param_count = 0; i < num_args; i++, arguments_copy++) {
				if (param_count > 0) {
					strncat(query_string, "&", 1);
				}
				strncat(query_string, arguments_copy->name, 20);
				strncat(query_string, "=", 1);
				strncat(query_string, arguments_copy->value, 64); // TODO need to escape this
				param_count++;
			}

			curl_easy_setopt(curl_handle, CURLOPT_POST, true);
			curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, query_string);
		} else {
			mime_handle = curl_mime_init(curl_handle);
			curl_mimepart *mime_part;

			arguments_copy = arguments;
			for (int i = 0; i < num_args; i++, arguments_copy++) {
				if (arguments_copy->file == NULL) {
					mime_part = curl_mime_addpart(mime_handle);
					curl_mime_name(mime_part, arguments_copy->name);
					curl_mime_data(mime_part, arguments_copy->value, CURL_ZERO_TERMINATED);
				} else if (arguments_copy->file) {
					fseek(arguments_copy->file, 0, SEEK_END);
					long length = ftell(arguments_copy->file);
					fseek(arguments_copy->file, 0, SEEK_SET);

					mime_part = curl_mime_addpart(mime_handle);
					curl_mime_name(mime_part, arguments_copy->name);
					curl_mime_filename(mime_part, arguments_copy->file_name);
					curl_mime_data_cb(mime_part, length, (curl_read_callback) fread,
					                  (curl_seek_callback) fseek, NULL, arguments_copy->file);
				}
			}

			curl_easy_setopt(curl_handle, CURLOPT_MIMEPOST, mime_handle);
			//curl_easy_setopt(curl_handle, CURLOPT_VERBOSE, 1);
		}

		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
		result_code = curl_easy_perform(curl_handle);

		if (result_code == CURLE_OK) { // TODO better error handling
			//printf("The cURL request was good: %s\n", chunk.memory);
		} else {
			printf("The cURL request went to heck\n");
		}

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
		char id_urlsafe[20] = {0};
		char query_string[512] = {0};

		for (int i = 0, param_count = 0; i < num_args; i++, arguments++) {
			if (arguments->file == NULL) {
				if (strcmp(arguments->name, "id") == 0) {
					char *id_o = curl_easy_escape(curl_handle, arguments->value, 0);
					strncpy(id_urlsafe, id_o, 20);
					curl_free(id_o);
				} else {
					if (param_count > 0) {
						strncat(query_string, "&", 1);
					}
					strncat(query_string, arguments->name, 20);
					strncat(query_string, "=", 1);
					strncat(query_string, arguments->value, 64); // TODO need to escape this
					param_count++;
				}
			}
		}

		char ws_url[300];
		strncpy(ws_url, base_url, 150);
		strncat(ws_url, "/", 1);
		strncat(ws_url, entity, 50);

		if (strlen(id_urlsafe) > 0) {
			strncat(ws_url, "?id=", 4);
			strncat(ws_url, id_urlsafe, 20);
		}

		curl_easy_setopt(curl_handle, CURLOPT_URL, ws_url);
		curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, query_string);

		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
		result_code = curl_easy_perform(curl_handle);

		if (result_code == CURLE_OK) { // TODO better error handling
			//printf("The cURL request was good: %s\n", chunk.memory);
		} else {
			printf("The cURL request went to heck\n");
		}
	} else if (strcmp(verb, "update") == 0) {
		char id_urlsafe[20] = {0};
		char query_string[512] = {0};

		for (int i = 0, param_count = 0; i < num_args; i++, arguments++) {
			if (arguments->file == NULL) {
				if (strcmp(arguments->name, "id") == 0) {
					char *id_o = curl_easy_escape(curl_handle, arguments->value, 0);
					strncpy(id_urlsafe, id_o, 20);
					curl_free(id_o);
				} else {
					if (param_count > 0) {
						strncat(query_string, "&", 1);
					}
					strncat(query_string, arguments->name, 20);
					strncat(query_string, "=", 1);
					strncat(query_string, arguments->value, 64); // TODO need to escape this
					param_count++;
				}
			}
		}

		char ws_url[300];
		strncpy(ws_url, base_url, 150);
		strncat(ws_url, "/", 1);
		strncat(ws_url, entity, 50);

		if (strlen(id_urlsafe) > 0) {
			strncat(ws_url, "?id=", 4);
			strncat(ws_url, id_urlsafe, 20);
		}

		curl_easy_setopt(curl_handle, CURLOPT_URL, ws_url);
		curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "PATCH");
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, query_string);

		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
		result_code = curl_easy_perform(curl_handle);

		if (result_code == CURLE_OK) { // TODO better error handling
			//printf("The cURL request was good: %s\n", chunk.memory);
		} else {
			printf("The cURL request went to heck\n");
		}
	} else if (strcmp(verb, "delete") == 0) {
		char id_urlsafe[20] = {0};

		for (int i = 0; i < num_args; i++, arguments++) {
			if (strcmp(arguments->name, "id") == 0) {
				char *id_o = curl_easy_escape(curl_handle, arguments->value, 0);
				strncpy(id_urlsafe, id_o, 20);
				curl_free(id_o);
			}
		}

		char ws_url[300];
		strncpy(ws_url, base_url, 150);
		strncat(ws_url, "/", 1);
		strncat(ws_url, entity, 50);

		if (strlen(id_urlsafe) > 0) {
			strncat(ws_url, "?id=", 4);
			strncat(ws_url, id_urlsafe, 20);
		}

		curl_easy_setopt(curl_handle, CURLOPT_URL, ws_url);
		curl_easy_setopt(curl_handle, CURLOPT_CUSTOMREQUEST, "DELETE");

		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
		result_code = curl_easy_perform(curl_handle);

		if (result_code == CURLE_OK) { // TODO better error handling
			//printf("The cURL request was good: %s\n", chunk.memory);
		} else {
			printf("The cURL request went to heck\n");
		}
	} else {
		char id_urlsafe[20] = {0};

		for (int i = 0; i < num_args; i++, arguments++) {
			if (strcmp(arguments->name, "id") == 0) {
				char *id_o = curl_easy_escape(curl_handle, arguments->value, 0);
				strncpy(id_urlsafe, id_o, 20);
				curl_free(id_o);
			}
		}

		char ws_url[300];
		strncpy(ws_url, base_url, 150);
		strncat(ws_url, "/", 1);
		strncat(ws_url, entity, 50);
		strncat(ws_url, "/", 1);
		strncat(ws_url, verb, 50);

		if (strlen(id_urlsafe) > 0) {
			strncat(ws_url, "?id=", 4);
			strncat(ws_url, id_urlsafe, 20);
		}

		curl_handle = curl_easy_init();
		curl_easy_setopt(curl_handle, CURLOPT_URL, ws_url);
		curl_easy_setopt(curl_handle, CURLOPT_POST, true);
		curl_easy_setopt(curl_handle, CURLOPT_POSTFIELDS, ""); // Empty string required if no actual POST data

		curl_easy_setopt(curl_handle, CURLOPT_WRITEFUNCTION, write_memory_callback);
		curl_easy_setopt(curl_handle, CURLOPT_WRITEDATA, (void *) &chunk);
		result_code = curl_easy_perform(curl_handle);

		if (result_code == CURLE_OK) { // TODO better error handling
			//printf("The cURL request was good: %s\n", chunk.memory);
		} else {
			printf("The cURL request went to heck\n");
		}
	}

	curl_easy_cleanup(curl_handle);
	curl_global_cleanup();
	return chunk.memory;
}