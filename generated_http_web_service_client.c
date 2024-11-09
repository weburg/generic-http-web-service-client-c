#include <cjson/cJSON.h>
#include <stdlib.h>

#include "weburg/ghowst/http_web_service_invoker.h"
#include "engine.h"
#include "photo.h"

#define NUM_ARGS 1
struct engine *get_engines(char *base_url, int id) {
    struct url_parameter arguments[NUM_ARGS];

    char value0[11];
    sprintf(value0, "%d", id);
    arguments[0].name = "id";
    arguments[0].value = value0;
    arguments[0].file = NULL;

    char *result = invoke(__FUNCTION__, arguments, (id != 0 ? NUM_ARGS : 0), base_url);

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
#undef NUM_ARGS

#define NUM_ARGS 3
int create_engines(char *base_url, struct engine engine)
{
    struct url_parameter arguments[NUM_ARGS];

    arguments[0].name = "name";
    arguments[0].value = engine.name;
    arguments[0].file = NULL;

    char value1[11];
    sprintf(value1, "%d", engine.cylinders);
    arguments[1].name = "cylinders";
    arguments[1].value = value1;
    arguments[1].file = NULL;

    char value2[11];
    sprintf(value2, "%d", engine.throttle_setting);
    arguments[2].name = "throttle_setting";
    arguments[2].value = value2;
    arguments[2].file = NULL;

    char *result = invoke(__FUNCTION__, arguments, NUM_ARGS, base_url);

    long result_id = strtol(result, (char **) NULL, 10);

    free(result);

    return result_id;
}
#undef NUM_ARGS

#define NUM_ARGS 4
int create_or_replace_engines(char *base_url, struct engine engine)
{
    struct url_parameter arguments[NUM_ARGS];

    char value0[11];
    sprintf(value0, "%d", engine.id);
    arguments[0].name = "id";
    arguments[0].value = value0;
    arguments[0].file = NULL;

    arguments[1].name = "name";
    arguments[1].value = engine.name;
    arguments[1].file = NULL;

    char value2[11];
    sprintf(value2, "%d", engine.cylinders);
    arguments[2].name = "cylinders";
    arguments[2].value = value2;
    arguments[2].file = NULL;

    char value3[11];
    sprintf(value3, "%d", engine.throttle_setting);
    arguments[3].name = "throttle_setting";
    arguments[3].value = value3;
    arguments[3].file = NULL;

    char *result = invoke(__FUNCTION__, arguments, NUM_ARGS, base_url);

    long result_id = strtol(result, (char **) NULL, 10);

    free(result);

    return result_id;
}
#undef NUM_ARGS

#define NUM_ARGS 4
void update_engines(char *base_url, struct engine engine)
{
    struct url_parameter arguments[NUM_ARGS];

    char value0[11];
    sprintf(value0, "%d", engine.id);
    arguments[0].name = "id";
    arguments[0].value = value0;
    arguments[0].file = NULL;

    arguments[1].name = "name";
    arguments[1].value = engine.name;
    arguments[1].file = NULL;

    char value2[11];
    sprintf(value2, "%d", engine.cylinders);
    arguments[2].name = "cylinders";
    arguments[2].value = value2;
    arguments[2].file = NULL;

    char value3[11];
    sprintf(value3, "%d", engine.throttle_setting);
    arguments[3].name = "throttle_setting";
    arguments[3].value = value3;
    arguments[3].file = NULL;

    char *result = invoke(__FUNCTION__, arguments, NUM_ARGS, base_url);
    free(result);
}
#undef NUM_ARGS

#define NUM_ARGS 1
void delete_engines(char *base_url, int id)
{
    struct url_parameter arguments[NUM_ARGS];

    char value0[11];
    sprintf(value0, "%d", id);
    arguments[0].name = "id";
    arguments[0].value = value0;
    arguments[0].file = NULL;

    char *result = invoke(__FUNCTION__, arguments, NUM_ARGS, base_url);
    free(result);
}
#undef NUM_ARGS

#define NUM_ARGS 1
void restart_engines(char *base_url, int id)
{
    struct url_parameter arguments[NUM_ARGS];

    char value0[11];
    sprintf(value0, "%d", id);
    arguments[0].name = "id";
    arguments[0].value = value0;
    arguments[0].file = NULL;

    char *result = invoke(__FUNCTION__, arguments, NUM_ARGS, base_url);
    free(result);
}
#undef NUM_ARGS

#define NUM_ARGS 2
char *create_photos(char *base_url, struct photo photo)
{
    struct url_parameter arguments[NUM_ARGS];

    arguments[0].name = "caption";
    arguments[0].value = photo.caption;
    arguments[0].file = NULL;

    arguments[1].name = "photo_file";
    arguments[1].value = NULL;
    arguments[1].file = photo.photo_file;
    arguments[1].file_name = photo.photo_file_name;

    return invoke(__FUNCTION__, arguments, NUM_ARGS, base_url);
}
#undef NUM_ARGS

#define NUM_ARGS 1
void play_sounds(char *base_url, char *name)
{
    struct url_parameter arguments[NUM_ARGS];

    arguments[0].name = "name";
    arguments[0].value = name;
    arguments[0].file = NULL;

    char *result = invoke("play_sounds", arguments, NUM_ARGS, base_url);
    free(result);
}
#undef NUM_ARGS