#ifndef PHOTO_H
#define PHOTO_H

#include <stdio.h>

typedef struct photo {
    char *name;
    char *caption;
    FILE *photo_file;
} photo_t;

#endif //PHOTO_H