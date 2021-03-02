//
// Created by filip on 28. 02. 2021..
//

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include "input.h"

int int_input(int *number) {
    long a;
    char buf[1024]; // use 1KiB just to be sure

    if (!fgets(buf, 1024, stdin)) {
        // reading input failed:
        return 0;
    }

    // have some input, convert it to integer:
    char *endptr;

    errno = 0; // reset error number
    a = strtol(buf, &endptr, 10);
    if (errno == ERANGE) {
        // out of range for a long
        return 0;
    }
    if (endptr == buf) {
        // no character was read
        return 0;
    }
    if (*endptr && *endptr != '\n') {
        // *endptr is neither end of string nor newline,
        // so we didn't convert the *whole* input
        return 0;
    }
    if (a > INT_MAX || a < INT_MIN) {
        // result will not fit in an int
        return 0;
    }

    // write result through the pointer passed
    *number = (int) a;
    return 1;
}

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

void read_input(InputBuffer *input_buffer) {
    ssize_t bytes_read =
            getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    // Ignore trailing newline
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}


InputBuffer *new_input_buffer() {
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}