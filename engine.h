#ifndef ENGINE_H
#define ENGINE_H

typedef struct engine {
    int id;
    char *name;
    int cylinders;
    int throttle_setting;
} engine_t;

#endif //ENGINE_H