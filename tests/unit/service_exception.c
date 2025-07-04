#include <assert.h>
#include <stdlib.h>

#include "../../generated_http_web_service_client.h"

int main(int argc, char *argv[])
{
    ghowsth ghowst_fail = ghowst_init("http://nohost:8081/noservice");

    engine_t engine = {
        .name = "CTestEngine",
        .cylinders = 12,
        .throttle_setting = 50
    };
    create_engines(ghowst_fail, engine);
    ghowsthttp_web_service_error_t error = ghowst_last_error(ghowst_fail);
    assert(error.error);

    ghowst_cleanup(ghowst_fail);

    return EXIT_SUCCESS;
}