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

#define LOG_FILE "passmannlog.txt"
#define INIT_MSG  "LOG FOR PASSMANN CREATED"
FILE *fp;

void create_log_file();
void display_log();
void append_log(char*date ,char* input);
bool check_log();

#endif //PASSMANN_LOG_H
