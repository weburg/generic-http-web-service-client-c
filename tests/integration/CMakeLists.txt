# Test via the example application
add_test(NAME integration_example_generic_http_web_service_client COMMAND run_example_generic_http_web_service_client WORKING_DIRECTORY $<TARGET_FILE_DIR:run_example_generic_http_web_service_client>)

set(SOURCE_FILES
        ../../generated_http_web_service_client.h
        ../../generated_http_web_service_client.c
        ../../weburg/ghowst/http_web_service_invoker.h
        ../../weburg/ghowst/http_web_service_invoker.c
        ../../engine.h
        ../../image.h
        ../../sound.h
        ../../truck.h
        ../../video.h
)
add_executable(create_engine create_engine.c ${SOURCE_FILES})
target_link_libraries(create_engine PRIVATE cjson CURL::libcurl)
add_test(NAME integration_create_engine COMMAND create_engine)