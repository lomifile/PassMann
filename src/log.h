//
// Created by filip on 25. 02. 2021..
//

#ifndef PASSMANN_LOG_H
#define PASSMANN_LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <time.h>
#include <malloc.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>

#define LOG_FILE "passmannlog.txt"
#define INIT_MSG "LOG FOR PASSMANN CREATED"

void create_log_file();
void display_log();
void append_log(char *date, char *input);
bool check_log();
char *time_now();
char *formated_string(char *input, ...);

#endif //PASSMANN_LOG_H
