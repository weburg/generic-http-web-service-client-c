#ifndef SOUND_H
#define SOUND_H

#include <stdio.h>

typedef struct sound {
    char *name;
    FILE *sound_file;
} sound;

#endif //SOUND_H