#include <stdio.h>
#include <stdlib.h>

#include "generated_http_web_service_client.h"
#include "engine.h"
#include "photo.h"

char *base_url = "http://localhost:8081/generichttpws";

int main(int argc, char *argv[])
{
    /*** Photo ***/

    // Create
    photo photo = {
            .caption = "Some C generated",
            .photo_file = fopen("../Sealion-e1517614416654.jpg", "rb"),
            .photo_file_name = "Sealion-e1517614416654.jpg"
    };
    char *photo_file_name = create_photos(base_url, photo);
    free(photo_file_name);

    /*** Engine ***/

    // Create
    engine engine_create = {
            .name = "CengineGenerated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id1 = create_engines(base_url, engine_create);

    // CreateOrReplace (which will create)
    engine engine_created_not_replaced = {
            .id = -1,
            .name = "CengineCreatedNotReplacedGenerated",
            .cylinders = 45,
            .throttle_setting = 50
    };
    create_or_replace_engines(base_url, engine_created_not_replaced);

    // Prepare for CreateOrReplace
    engine engine_create2 = {
            .name = "Cengine2Generated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id2 = create_engines(base_url, engine_create2);

    // CreateOrReplace (which will replace)
    engine engine_replacement = {
            .id = engine_id2,
            .name = "CengineReplacement",
            .cylinders = 56,
            .throttle_setting = 59
    };
    create_or_replace_engines(base_url, engine_replacement);

    // Prepare for Update
    engine engine_create3 = {
            .name = "Cengine3Generated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id3 = create_engines(base_url, engine_create3);

    // Update
    engine engine_update = {
            .id = engine_id3,
            .name = "Cengine3GeneratedUpdated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    update_engines(base_url, engine_update);

    // Get
    engine *engine = get_engines(base_url, engine_id1);
    printf("Engine returned: %s\n", engine->name);
    free(engine);

    // Get all
    struct engine *engines = get_engines(base_url, 0);
    struct engine *engines_copy = engines;

    int count = 0;
    for (; engines_copy->id != '\0'; engines_copy++, count++);

    printf("Engines returned: %d\n", count);
    free(engines);

    // Prepare for delete
    struct engine engine_to_delete = {
            .name = "CengineToDelete",
            .cylinders = 89,
            .throttle_setting = 70
    };
    int engine_id4 = create_engines(base_url, engine_to_delete);

    // Delete
    delete_engines(base_url, engine_id4);

    // Custom verb
    restart_engines(base_url, engine_id2);

    return EXIT_SUCCESS;
}