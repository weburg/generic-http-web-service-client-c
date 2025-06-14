#include <stdlib.h>

#include "../../generated_http_web_service_client.h"

int main(int argc, char *argv[])
{
    ghowsth ghowst_fail = ghowst_init("http://nohost:8081/noservice");

    // Induce a service error and catch it
    get_engines(ghowst_fail, -2);
    ghowsthttp_web_service_error_t error = ghowst_last_error(ghowst_fail);
    if (error.error) {
        return EXIT_SUCCESS;
    }

    ghowst_cleanup(ghowst_fail);

    return EXIT_FAILURE;
}