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
add_executable(service_exception service_exception.c ${SOURCE_FILES})
target_link_libraries(service_exception PRIVATE cjson CURL::libcurl)

add_test(NAME unit_service_exception COMMAND service_exception)