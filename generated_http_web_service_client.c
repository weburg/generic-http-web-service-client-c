#include <cjson/cJSON.h>
#include <stdlib.h>

#include "weburg/ghowst/http_web_service_invoker.h"
#include "engine.h"
#include "photo.h"

struct engine *get_engines(char *base_url, int id) {
    struct url_parameter arguments[1];

    char value[11];
    sprintf(value, "%d", id);
    arguments[0].name = "id";
    arguments[0].value = value;
    arguments[0].file = NULL;

    char *result = invoke("get_engines", arguments, 1, base_url);

    cJSON *json = cJSON_Parse(result);
    int num_results = 0;

    if (cJSON_IsObject(json)) {
        num_results = 1;
    } else if (cJSON_IsArray(json)) {
        num_results = cJSON_GetArraySize(json);
    }

    struct engine *engines = malloc(sizeof(struct engine) * (num_results + 1));

    struct engine *engines_fill = engines;

    if (num_results && cJSON_IsObject(json)) {
        engines_fill->id = cJSON_GetObjectItemCaseSensitive(json, "id")->valueint;
        engines_fill->name = cJSON_GetObjectItemCaseSensitive(json, "name")->valuestring;
        engines_fill->cylinders = cJSON_GetObjectItemCaseSensitive(json, "cylinders")->valueint;
        engines_fill->throttle_setting = cJSON_GetObjectItemCaseSensitive(json, "throttleSetting")->valueint;
        engines_fill++;
    } else if (num_results && cJSON_IsArray(json)) {
        const cJSON *engine = NULL;
        cJSON_ArrayForEach(engine, json) {
            engines_fill->id = cJSON_GetObjectItemCaseSensitive(engine, "id")->valueint;
            engines_fill->name = cJSON_GetObjectItemCaseSensitive(engine, "name")->valuestring;
            engines_fill->cylinders = cJSON_GetObjectItemCaseSensitive(engine, "cylinders")->valueint;
            engines_fill->throttle_setting = cJSON_GetObjectItemCaseSensitive(engine, "throttleSetting")->valueint;
            engines_fill++;
        }
    }

    engines_fill->id = '\0'; // Sentinel

    free(result);
    // Don't call cJSON_Delete(json), its value was added to an array; if we delete it here it impacts returned value
    // Instead, the caller will free the returned value, which cJSON says will in turn free cjson

    return engines;
}

int create_engines(char *base_url, struct engine engine)
{
	struct url_parameter arguments[3];

	arguments[0].name = "name";
	arguments[0].value = engine.name;
	arguments[0].file = NULL;

	char cylinders[11];
	sprintf(cylinders, "%d", engine.cylinders);
	arguments[1].name = "cylinders";
	arguments[1].value = cylinders;
	arguments[1].file = NULL;

	char throttle_setting[11];
	sprintf(throttle_setting, "%d", engine.throttle_setting);
	arguments[2].name = "throttleSetting"; // TODO perform dynamic conversion
	arguments[2].value = throttle_setting;
	arguments[2].file = NULL;

	char *result = invoke("create_engines", arguments, 3, base_url);

	long result_id = strtol(result, (char **) NULL, 10);

	free(result);

	return result_id;
}

int create_or_replace_engines(char *base_url, struct engine engine)
{
	struct url_parameter arguments[4];

	char id[11];
	sprintf(id, "%d", engine.id);
	arguments[0].name = "id";
	arguments[0].value = id;
	arguments[0].file = NULL;

	arguments[1].name = "name";
	arguments[1].value = engine.name;
	arguments[1].file = NULL;

	char cylinders[11];
	sprintf(cylinders, "%d", engine.cylinders);
	arguments[2].name = "cylinders";
	arguments[2].value = cylinders;
	arguments[2].file = NULL;

	char throttle_setting[11];
	sprintf(throttle_setting, "%d", engine.throttle_setting);
	arguments[3].name = "throttleSetting"; // TODO perform dynamic conversion
	arguments[3].value = throttle_setting;
	arguments[3].file = NULL;

	char *result = invoke("create_or_replace_engines", arguments, 4, base_url);

	long result_id = strtol(result, (char **) NULL, 10);

	free(result);

	return result_id;
}

void update_engines(char *base_url, struct engine engine)
{
	struct url_parameter arguments[4];

	char id[11];
	sprintf(id, "%d", engine.id);
	arguments[0].name = "id";
	arguments[0].value = id;
	arguments[0].file = NULL;

	arguments[1].name = "name";
	arguments[1].value = engine.name;
	arguments[1].file = NULL;

	char cylinders[11];
	sprintf(cylinders, "%d", engine.cylinders);
	arguments[2].name = "cylinders";
	arguments[2].value = cylinders;
	arguments[2].file = NULL;

	char throttle_setting[11];
	sprintf(throttle_setting, "%d", engine.throttle_setting);
	arguments[3].name = "throttleSetting"; // TODO perform dynamic conversion
	arguments[3].value = throttle_setting;
	arguments[3].file = NULL;

	char *result = invoke("update_engines", arguments, 4, base_url);
	free(result);
}

void delete_engines(char *base_url, int id)
{
	struct url_parameter arguments[1];

	char id_str[11];
	sprintf(id_str, "%d", id);
	arguments[0].name = "id";
	arguments[0].value = id_str;
	arguments[0].file = NULL;

	char *result = invoke("delete_engines", arguments, 1, base_url);
	free(result);
}

void restart_engines(char *base_url, int id)
{
	struct url_parameter arguments[1];

	char id_str[11];
	sprintf(id_str, "%d", id);
	arguments[0].name = "id";
	arguments[0].value = id_str;
	arguments[0].file = NULL;

	char *result = invoke("restart_engines", arguments, 1, base_url);
	free(result);
}

char *create_photos(char *base_url, struct photo photo)
{
	struct url_parameter arguments[2];

	arguments[0].name = "caption";
	arguments[0].value = photo.caption;
	arguments[0].file = NULL;

	arguments[1].name = "photoFile"; // TODO perform dynamic conversion
	arguments[1].value = NULL;
	arguments[1].file = photo.photo_file;
	arguments[1].file_name = photo.photo_file_name;

	return invoke("create_photos", arguments, 2, base_url);
}