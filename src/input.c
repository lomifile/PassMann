#include "input.h"

int int_input(int *number)
{
    long a;
    char buf[1024]; // use 1KiB just to be sure

    if (!fgets(buf, 1024, stdin))
    {
        // reading input failed:
        append_log(time_now(), "Reading INTEGER input failed!");
        return 0;
    }

    // have some input, convert it to integer:
    char *endptr;

    errno = 0; // reset error number
    a = strtol(buf, &endptr, 10);
    if (errno == ERANGE)
    {
        // out of range for a long
        append_log(time_now(), "Out of range for a long");
        return 0;
    }
    if (endptr == buf)
    {
        // no character was read
        append_log(time_now(), "No character was read");
        return 0;
    }
    if (*endptr && *endptr != '\n')
    {
        // *endptr is neither end of string nor newline,
        // so we didn't convert the *whole* input
        append_log(time_now(), "Endptr is neither end of string nor newline, so we didn't convert the *whole* input");
        return 0;
    }
    if (a > INT_MAX || a < INT_MIN)
    {
        // result will not fit in an int
        return 0;
    }

    // write result through the pointer passed
    *number = (int)a;
    return 1;
}

char *read_line(FILE *f)
{
    int cap = 32, next = 0, c;
    char *p = malloc(cap);
    while (1)
    {
        if (next == cap)
        {
            p = realloc(p, cap *= 2);
        }
        c = fgetc(f);
        if (c == EOF || c == '\n')
        {
            p[next++] = 0;
            break;
        }
        p[next++] = c;
    }
    if (c == EOF && next == 1)
    {
        free(p);
        p = NULL;
    }

    return p;
}

void close_input_buffer(InputBuffer *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}

void read_input(InputBuffer *input_buffer)
{
    ssize_t bytes_read =
        getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0)
    {
        printf("Error reading input\n");
        append_log(time_now(), "Error reading input!");
        exit(EXIT_FAILURE);
    }

    // Ignore trailing newline
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

InputBuffer *new_input_buffer()
{
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}
