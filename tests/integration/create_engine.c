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

    assert(!ghowst_last_error(ghowst).error);
    assert(engine_id > 0);

    ghowst_cleanup(ghowst);

    return EXIT_SUCCESS;
}