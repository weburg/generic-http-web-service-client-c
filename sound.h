#ifndef SOUND_H
#define SOUND_H

#include <stdio.h>

typedef struct sound {
    char *name;
    FILE *sound_file;
} sound_t;

#endif //SOUND_H