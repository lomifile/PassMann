#ifndef PASSMANN_INPUT_H
#define PASSMANN_INPUT_H

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <errno.h>
#include <stdarg.h>

#include "log.h"

typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

void close_input_buffer(InputBuffer *input_buffer);

void read_input(InputBuffer *input_buffer);

InputBuffer *new_input_buffer();

int int_input(int *input);

char *read_line(FILE *f);

#endif //PASSMANN_INPUT_H
