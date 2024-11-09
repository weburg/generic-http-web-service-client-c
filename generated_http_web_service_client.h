#ifndef GENERATED_HTTP_WEB_SERVICE_CLIENT_H
#define GENERATED_HTTP_WEB_SERVICE_CLIENT_H

#include "engine.h"
#include "photo.h"

struct engine *get_engines(char *base_url, int id);

int create_engines(char *base_url, struct engine engine);

int create_or_replace_engines(char *base_url, struct engine engine);

void update_engines(char *base_url, struct engine engine);

void delete_engines(char *base_url, int id);

void restart_engines(char *base_url, int id);

char *create_photos(char *base_url, struct photo photo);

void play_sounds(char *base_url, char *name);

#endif //GENERATED_HTTP_WEB_SERVICE_CLIENT_H