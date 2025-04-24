#ifndef HTTP_WEB_SERVICE_INVOKER_H
#define HTTP_WEB_SERVICE_INVOKER_H

#include <stdio.h>

typedef void ghowsth;

typedef struct ghowsthttp_web_service_error {
    _Bool error; // Will be false for all successful calls
    long http_status; // If there is an error, this will contain the HTTP status
    char *message; // If there is an error, this will contain the server message
} ghowsthttp_web_service_error_t;

typedef struct ghowst_url_parameter {
    char *name;

    // For non-FILE types
    char *value;

    // Only for a file upload, set file to NULL for non_FILE types
    FILE *file;
    char *file_name;
} ghowst_url_parameter_t;

char *ghowst_invoke(ghowsth *ghowsth, const char *method_name, ghowst_url_parameter_t *arguments, int num_args);

ghowsthttp_web_service_error_t ghowst_last_error(ghowsth *ghowsth);

ghowsth *ghowst_init(char *base_url);

void ghowst_cleanup(ghowsth *ghowsth);

#endif //HTTP_WEB_SERVICE_INVOKER_H