cmake_minimum_required(VERSION 3.6)

project(generic_http_web_service_client_c)

set(CMAKE_C_STANDARD 11)

find_package(cJSON CONFIG REQUIRED)
find_package(CURL CONFIG REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB REQUIRED IMPORTED_TARGET glib-2.0)

add_library(ghowst
        weburg/ghowst/http_web_service_invoker.h
        weburg/ghowst/http_web_service_invoker.c
)

target_link_libraries(ghowst
        PUBLIC
        cjson
        CURL::libcurl
        PkgConfig::GLIB
)

set(SOURCE_FILES
        example_http_web_service_client.h
        example_http_web_service_client.c
        run_example_http_web_service_client.c
        engine.h
        image.h
        sound.h
        truck.h
        video.h)
add_executable(run_example_generic_http_web_service_client ${SOURCE_FILES})
target_link_libraries(run_example_generic_http_web_service_client PRIVATE ghowst)

# CLion
# Add to CMake options for each profile:
# -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake

enable_testing()
add_subdirectory(tests/unit)
add_subdirectory(tests/integration)