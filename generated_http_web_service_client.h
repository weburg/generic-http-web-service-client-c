#ifndef GENERATED_HTTP_WEB_SERVICE_CLIENT_H
#define GENERATED_HTTP_WEB_SERVICE_CLIENT_H

#include "weburg/ghowst/http_web_service_invoker.h"
#include "engine.h"
#include "photo.h"
#include "truck.h"

engine *get_engines(ghowst_handle *ghowst, int id);

int create_engines(ghowst_handle *ghowst, engine engine);

int create_or_replace_engines(ghowst_handle *ghowst, engine engine);

void update_engines(ghowst_handle *ghowst, engine engine);

void delete_engines(ghowst_handle *ghowst, int id);

void restart_engines(ghowst_handle *ghowst, int id);

char *create_photos(ghowst_handle *ghowst, photo photo);

void play_sounds(ghowst_handle *ghowst, char *name);

int race_trucks(ghowst_handle *ghowst, truck truck1, truck truck2);

#endif //GENERATED_HTTP_WEB_SERVICE_CLIENT_H