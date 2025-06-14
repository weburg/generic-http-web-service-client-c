# Generic HTTP Web Service Client in C (GHoWSt)

## A client written to talk to the Generic HTTP Web Service Server

### Design goals

- Use local language semantics to talk to the server without an intermediate
  language. Unlike with dynamic languages though, the C client will need to be
  generated manually, but beyond that, the only other things needed are domain
  object structs, the ghowst functions, and 3rd party dependencies from
  vcpkg.json.
- Every call, using a method name convention to map to HTTP methods, gets
  translated to HTTP requests. Responses are parsed from JSON and mapped back to
  local structs.

### Example code

```c
#include <stdlib.h>

#include "generated_http_web_service_client.h"

int main(int argc, char *argv[])
{
    ghowsth ghowst = ghowst_init("http://localhost:8081/generichttpws");

    // Create
    engine_t engine = {
            .name = "CengineGenerated",
            .cylinders = 44,
            .throttle_setting = 49
    };
    int engine_id1 = create_engines(ghowst, engine);

    return EXIT_SUCCESS;
}
```

### Running the example

First, ensure the server is running. Refer to other grouped GHoWSt projects to
get and run the server. Ensure a compiler (GCC, MSVC were tested) is installed.
Ensure vcpkg is installed (it doesn't have to be on the PATH for the CLI build
to work). Ensure CMake is installed.

If using the CLI, ensure you are in the project directory. Run:

```
cmake -S . -B build "-DCMAKE_TOOLCHAIN_FILE=<vcpkg root>/scripts/buildsystems/vcpkg.cmake"
cmake --build build
cd build
./run_example_generic_http_web_service_client
```

> [!NOTE]
> Change `<vcpkg root>` to match your installation. The last command's forward slash should be a backslash if on Windows.

If using an IDE, ensure it supports CMake and that the project profile
configures CMAKE_TOOLCHAIN_FILE to use vcpkg. Now, you should only need to run
the below file which should, after the CMake project builds, compile and run for
you:

`run_example_generic_http_web_service_client.c`

The example runs several calls to create, update, replace, read, delete, and do
a custom action on resources.

### Running the tests

To run unit tests only:

`ctest -R ^unit`

To run unit and integration tests:

`ctest`