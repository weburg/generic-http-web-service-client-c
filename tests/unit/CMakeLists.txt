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
        unit_tests.c)
add_executable(unit_tests ${SOURCE_FILES})
target_link_libraries(unit_tests PRIVATE cjson CURL::libcurl)

add_test(NAME unit_tests COMMAND unit_tests)