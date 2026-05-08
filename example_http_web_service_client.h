#ifndef EXAMPLE_HTTP_WEB_SERVICE_CLIENT_H
#define EXAMPLE_HTTP_WEB_SERVICE_CLIENT_H

#include "weburg/ghowst/http_web_service_invoker.h"
#include "engine.h"
#include "image.h"
#include "sound.h"
#include "video.h"
#include "truck.h"

sound_t *get_sounds(ghowsth ghowst, char *name, int *count);

char *create_sounds(ghowsth ghowst, sound_t sound);

void play_sounds(ghowsth ghowst, char *name);

image_t *get_images(ghowsth ghowst, char *name, int *count);

char *create_images(ghowsth ghowst, image_t image);

video_t *get_videos(ghowsth ghowst, char *name, int *count);

char *create_videos(ghowsth ghowst, video_t video);

engine_t *get_engines(ghowsth ghowst, int id, int *count);

int create_engines(ghowsth ghowst, engine_t engine);

int create_or_replace_engines(ghowsth ghowst, engine_t engine);

int update_engines(ghowsth ghowst, engine_t engine);

void delete_engines(ghowsth ghowst, int id);

int restart_engines(ghowsth ghowst, int id);

int stop_engines(ghowsth ghowst, int id);

char *race_trucks(ghowsth ghowst, truck_t truck1, truck_t truck2);

#endif //EXAMPLE_HTTP_WEB_SERVICE_CLIENT_H