#ifndef GENERATED_HTTP_WEB_SERVICE_CLIENT_H
#define GENERATED_HTTP_WEB_SERVICE_CLIENT_H

#include "engine.h"
#include "photo.h"

engine *get_engines(char *base_url, int id);

int create_engines(char *base_url, engine engine);

int create_or_replace_engines(char *base_url, engine engine);

void update_engines(char *base_url, engine engine);

void delete_engines(char *base_url, int id);

void restart_engines(char *base_url, int id);

char *create_photos(char *base_url, photo photo);

void play_sounds(char *base_url, char *name);

#endif //GENERATED_HTTP_WEB_SERVICE_CLIENT_H