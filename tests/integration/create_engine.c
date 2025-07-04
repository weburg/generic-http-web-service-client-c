#include <assert.h>
#include <stdlib.h>

#include "../../generated_http_web_service_client.h"

int main(int argc, char *argv[])
{
    ghowsth ghowst = ghowst_init("http://localhost:8081/generichttpws");

    engine_t engine = {
        .name = "CTestEngine",
        .cylinders = 12,
        .throttle_setting = 50
    };
    int engine_id = create_engines(ghowst, engine);
    ghowsthttp_web_service_error_t error = ghowst_last_error(ghowst);

    assert(error.error == 0);
    assert(engine_id > 0);

    ghowst_cleanup(ghowst);

    return EXIT_SUCCESS;
}