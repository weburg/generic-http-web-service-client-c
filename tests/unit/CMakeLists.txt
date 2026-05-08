set(SOURCE_FILES
        ../../example_http_web_service_client.h
        ../../example_http_web_service_client.c
        ../../engine.h
        ../../image.h
        ../../sound.h
        ../../truck.h
        ../../video.h
)
add_executable(service_exception service_exception.c ${SOURCE_FILES})
target_link_libraries(service_exception PRIVATE ghowst)

add_test(NAME unit_service_exception COMMAND service_exception)