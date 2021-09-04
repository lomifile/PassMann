//
// Created by filip on 28. 02. 2021..
//

#ifndef PASSMANN_INPUT_H
#define PASSMANN_INPUT_H

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
