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