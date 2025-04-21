#ifndef HTTP_WEB_SERVICE_INVOKER_H
#define HTTP_WEB_SERVICE_INVOKER_H

#include "curl/curl.h"
#include <stdio.h>

typedef struct http_web_service_error {
    _Bool error; // Will be false for all successful calls
    long http_status; // If there is an error, this will contain the HTTP status
    char *message; // If there is an error, this will contain the server message
} http_web_service_error;

typedef struct ghowst_handle {
    char *base_url;
    CURL *curl_handle;
    http_web_service_error last_http_web_service_error;
} ghowst_handle;

typedef struct url_parameter {
    char *name;

    // For non-FILE types
    char *value;

    // Only for a file upload, set file to NULL for non_FILE types
    FILE *file;
    char *file_name;
} url_parameter;

char *_underbar_to_camel(char *dest, const char *str);

char *invoke(const char *method_name, url_parameter *arguments, int num_args, ghowst_handle *ghowst);

http_web_service_error ghowst_last_error(ghowst_handle *ghowst);

void *ghowst_init(char *base_url);

void ghowst_cleanup(ghowst_handle *ghowst);

#endif //HTTP_WEB_SERVICE_INVOKER_H