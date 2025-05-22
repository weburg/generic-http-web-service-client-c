#ifndef VIDEO_H
#define VIDEO_H

#include <stdio.h>

typedef struct video {
    char *name;
    char *caption;
    FILE *video_file;
} video_t;

#endif //VIDEO_H