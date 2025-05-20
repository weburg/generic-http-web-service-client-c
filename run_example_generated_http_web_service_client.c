#include <stdlib.h>
#include <stdio.h>

#include "generated_http_web_service_client.h"

int main(int argc, char *argv[])
{
    ghowsth ghowst = ghowst_init("http://localhost:8081/generichttpws");

    /*** Image ***/

    // Create
    image_t image = {
            .name = "Sealion-e1517614416654.jpg",
            .caption = "Some C",
            .image_file = fopen("../Sealion-e1517614416654.jpg", "rb")
    };
    char *image_file_name = create_images(ghowst, image);
    free(image_file_name);

    /*** Engine ***/

    // Create
    engine_t engine_create = {
            .name = "Cengine",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id1 = create_engines(ghowst, engine_create);

    // CreateOrReplace (which will create)
    engine_t engine_created_not_replaced = {
            .id = -1,
            .name = "CengineCreatedNotReplaced",
            .cylinders = 45,
            .throttle_setting = 50
    };

    create_or_replace_engines(ghowst, engine_created_not_replaced);

    // Prepare for CreateOrReplace
    engine_t engine_create2 = {
            .name = "Cengine2",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id2 = create_engines(ghowst, engine_create2);

    // CreateOrReplace (which will replace)
    engine_t engine_replacement = {
            .id = engine_id2,
            .name = "Cengine2Replacement",
            .cylinders = 56,
            .throttle_setting = 59
    };
    create_or_replace_engines(ghowst, engine_replacement);

    // Prepare for Update
    engine_t engine_create3 = {
            .name = "Cengine3",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id3 = create_engines(ghowst, engine_create3);

    // Update
    engine_t engine_update = {
            .id = engine_id3,
            .name = "Cengine3Updated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    update_engines(ghowst, engine_update);

    // Get
    engine_t *engine = get_engines(ghowst, engine_id1);
    printf("Engine returned: %s\n", engine->name);
    free(engine);

    // Get all
    engine_t *engines = get_engines(ghowst, 0);
    engine_t *engines_copy = engines;

    int count = 0;
    for (; engines_copy->id != '\0'; engines_copy++, count++);

    printf("Engines returned: %d\n", count);
    free(engines);

    // Prepare for delete
    engine_t engine_to_delete = {
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

    truck_t truck1 = {
            .name = "Ram",
            .engine_id = engine_id1
    };
    truck_t truck2 = {
            .name = "Ford",
            .engine_id = engine_id2
    };
    char *truck_result = race_trucks(ghowst, truck1, truck2);

    printf("Race result: %s\n", truck_result);
    free(truck_result);

    // Induce a not found error and catch it

    engine_t *engine_2 = get_engines(ghowst, -2);
    ghowsthttp_web_service_error_t error = ghowst_last_error(ghowst);
    if (!error.error) {
        printf("Engine returned: %s\n", engine_2->name);
    } else {
        printf("Status: %ld Message: %s\n", error.http_status, error.message);
    }

    ghowsth ghowst_fail = ghowst_init("http://nohost:8081/generichttpws");

    // Induce a service error and catch it
    get_engines(ghowst_fail, -2);
    ghowsthttp_web_service_error_t error2 = ghowst_last_error(ghowst_fail);
    if (error2.error) {
        printf("Status: %ld Message: %s\n", error2.http_status, error2.message);
    }

    ghowst_cleanup(ghowst);
    ghowst_cleanup(ghowst_fail);

    return EXIT_SUCCESS;
}