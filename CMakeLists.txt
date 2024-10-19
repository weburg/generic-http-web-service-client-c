cmake_minimum_required(VERSION 3.6)

project(generic_http_web_service_client_c)
set(CMAKE_C_STANDARD 11)
set(SOURCE_FILES generated_http_web_service_client.c weburg/ghowst/http_status.h weburg/ghowst/http_web_service_invoker.c weburg/ghowst/http_web_service_invoker.h
        run_example_generated_http_web_service_client.c
        generated_http_web_service_client.h
        photo.h
        engine.h)
add_executable(run_example_generic_http_web_service_client ${SOURCE_FILES})
target_link_libraries(run_example_generic_http_web_service_client PRIVATE cjson CURL::libcurl)

# CLion
# Add to CMake options for each profile:
# -DCMAKE_TOOLCHAIN_FILE=[vcpkg root]/scripts/buildsystems/vcpkg.cmake

# vcpkg install cJSON
find_package(cJSON CONFIG REQUIRED)

# vcpkg install curl curl[ssl]
find_package(CURL CONFIG REQUIRED)