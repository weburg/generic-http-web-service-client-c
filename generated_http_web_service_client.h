#ifndef GENERATED_HTTP_WEB_SERVICE_CLIENT_H
#define GENERATED_HTTP_WEB_SERVICE_CLIENT_H

#include "weburg/ghowst/http_web_service_invoker.h"
#include "engine.h"
#include "photo.h"
#include "sound.h"
#include "truck.h"

engine *get_engines(ghowsth *ghowst, int id);

int create_engines(ghowsth *ghowst, engine engine);

int create_or_replace_engines(ghowsth *ghowst, engine engine);

void update_engines(ghowsth *ghowst, engine engine);

void delete_engines(ghowsth *ghowst, int id);

void restart_engines(ghowsth *ghowst, int id);

void stop_engines(int id);

photo *get_photos(char *name);

char *create_photos(ghowsth *ghowst, photo photo);

sound *get_sounds(char *name);

char *create_sounds(sound sound);

void play_sounds(ghowsth *ghowst, char *name);

int race_trucks(ghowsth *ghowst, truck truck1, truck truck2);

#endif //GENERATED_HTTP_WEB_SERVICE_CLIENT_H