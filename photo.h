#ifndef PHOTO_H
#define PHOTO_H

#include <stdio.h>

typedef struct photo {
    char *caption;
    FILE *photo_file;
    char *photo_file_name;
} photo;

#endif //PHOTO_H