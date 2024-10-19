#ifndef PHOTO_H
#define PHOTO_H

#include <stdio.h>

struct photo {
    char *caption;
    FILE *photo_file;
    char *photo_file_name;
};

#endif //PHOTO_H
