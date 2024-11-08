#ifndef HTTP_WEB_SERVICE_INVOKER_H
#define HTTP_WEB_SERVICE_INVOKER_H

#include <stdio.h>

struct url_parameter {
	char *name;

	// For non-FILE types
	char *value;

	// Only for a file upload, set file to NULL for non_FILE types
	FILE *file;
	char *file_name;
};

char *_underbar_to_camel(char *dest, const char *str);

char *invoke(const char *method_name, struct url_parameter *arguments, int num_args, char *base_url);

#endif //HTTP_WEB_SERVICE_INVOKER_H
