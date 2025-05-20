#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>

typedef struct image {
    char *name;
    char *caption;
    FILE *image_file;
} image_t;

#endif //IMAGE_H