#ifndef HTTP_WEB_SERVICE_INVOKER_H
#define HTTP_WEB_SERVICE_INVOKER_H

#include "curl/curl.h"
#include <stdio.h>

typedef void GHOWST;

typedef struct ghowst_http_web_service_error {
    _Bool error; // Will be false for all successful calls
    long http_status; // If there is an error, this will contain the HTTP status
    char *message; // If there is an error, this will contain the server message
} ghowst_http_web_service_error;

typedef struct ghowst_url_parameter {
    char *name;

    // For non-FILE types
    char *value;

    // Only for a file upload, set file to NULL for non_FILE types
    FILE *file;
    char *file_name;
} ghowst_url_parameter;

char *ghowst_invoke(GHOWST *ghowsth, const char *method_name, ghowst_url_parameter *arguments, int num_args);

ghowst_http_web_service_error ghowst_last_error(GHOWST *ghowsth);

void *ghowst_init(char *base_url);

void ghowst_cleanup(GHOWST *ghowsth);

#endif //HTTP_WEB_SERVICE_INVOKER_H