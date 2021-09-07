//
// Created by filip on 25. 02. 2021..
//

#ifndef PASSMANN_LOG_H
#define PASSMANN_LOG_H

#include <stdbool.h>

#define LOG_FILE "passmannlog.txt"
#define INIT_MSG "LOG FOR PASSMANN CREATED"

void create_log_file();
void display_log();
void append_log(const char *date, const char *input);
bool check_log();
char *time_now();
char *formated_string(char *input, ...);

#endif //PASSMANN_LOG_H
