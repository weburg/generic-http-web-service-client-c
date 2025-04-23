#include <stdio.h>
#include <stdlib.h>

#include "generated_http_web_service_client.h"

int main(int argc, char *argv[])
{
    ghowsth *ghowst = ghowst_init("http://localhost:8081/generichttpws");

    /*** Photo ***/

    // Create
    photo photo = {
            .name = "Sealion-e1517614416654.jpg",
            .caption = "Some C generated",
            .photo_file = fopen("../Sealion-e1517614416654.jpg", "rb")
    };
    char *photo_file_name = create_photos(ghowst, photo);
    free(photo_file_name);

    /*** Engine ***/

    // Create
    engine engine_create = {
            .name = "CengineGenerated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id1 = create_engines(ghowst, engine_create);

    // CreateOrReplace (which will create)
    engine engine_created_not_replaced = {
            .id = -1,
            .name = "CengineCreatedNotReplacedGenerated",
            .cylinders = 45,
            .throttle_setting = 50
    };
    create_or_replace_engines(ghowst, engine_created_not_replaced);

    // Prepare for CreateOrReplace
    engine engine_create2 = {
            .name = "Cengine2Generated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id2 = create_engines(ghowst, engine_create2);

    // CreateOrReplace (which will replace)
    engine engine_replacement = {
            .id = engine_id2,
            .name = "CengineReplacement",
            .cylinders = 56,
            .throttle_setting = 59
    };
    create_or_replace_engines(ghowst, engine_replacement);

    // Prepare for Update
    engine engine_create3 = {
            .name = "Cengine3Generated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id3 = create_engines(ghowst, engine_create3);

    // Update
    engine engine_update = {
            .id = engine_id3,
            .name = "Cengine3GeneratedUpdated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    update_engines(ghowst, engine_update);

    // Get
    engine *engine = get_engines(ghowst, engine_id1);
    printf("Engine returned: %s\n", engine->name);
    free(engine);

    // Get all
    struct engine *engines = get_engines(ghowst, 0);
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
    int engine_id4 = create_engines(ghowst, engine_to_delete);

    // Delete
    delete_engines(ghowst, engine_id4);

    // Custom verb
    restart_engines(ghowst, engine_id2);

    // Repeat, complex objects with different names

    struct truck truck1 = {
            .name = "Ram",
            .engine_id = engine_id1
    };
    struct truck truck2 = {
            .name = "Ford",
            .engine_id = engine_id2
    };
    int truck_name_compare_result = race_trucks(ghowst, truck1, truck2);

    if (truck_name_compare_result == 0) {
        printf("Did not expect both trucks to have the same name.\n");
    }

    // Induce a not found error and catch it

    struct engine *engine_2 = get_engines(ghowst, -2);
    ghowsthttp_web_service_error error = ghowst_last_error(ghowst);
    if (!error.error) {
        printf("Engine returned: %s\n", engine_2->name);
    } else {
        printf("Status: %ld Message: %s\n", error.http_status, error.message);
    }

    ghowsth *ghowst_fail = ghowst_init("http://nohost:8081/generichttpws");

    // Induce a service error and catch it
    get_engines(ghowst_fail, -2);
    ghowsthttp_web_service_error error2 = ghowst_last_error(ghowst_fail);
    if (error2.error) {
        printf("Status: %ld Message: %s\n", error2.http_status, error2.message);
    }

    ghowst_cleanup(ghowst);
    ghowst_cleanup(ghowst_fail);

    return EXIT_SUCCESS;
}