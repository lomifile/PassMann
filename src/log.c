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

#include "log.h"

void create_log_file()
{
    FILE *fp = fopen(LOG_FILE, "w");
    char t_now[30];
    if (fp == NULL)
        exit(EXIT_FAILURE);

    time_t t = time(NULL);
    fprintf(fp, "%s --------> %s", INIT_MSG, ctime(&t));
    fclose(fp);
}

void display_log()
{
    FILE *fp = fopen(LOG_FILE, "r");
    if (fp == NULL)
    {
        printf("There was a problem!");
        exit(EXIT_FAILURE);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while ((read = getline(&line, &len, fp)) != -1)
    {
        printf("%s", line);
    }
    fclose(fp);
    if (line)
        free(line);
}

void append_log(const char *date, const char *input)
{
    strtok((char *)date, "\n");
    FILE *fp = fopen(LOG_FILE, "a");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    fprintf(fp, "%s --------> %s\n", date, input);
    fclose(fp);
}

bool check_log()
{
    if (access(LOG_FILE, F_OK) != -1)
        return true;
    else
        return false;
}

char *formated_string(char *format, ...)
{
    char *string = (char *)malloc(sizeof format);
    int done;
    va_list args;
    va_start(args, format);
    done = vsnprintf(string, (uint32_t)strlen(format), format, args);
    va_end(args);
    return string;
}

char *time_now()
{
    time_t now = time(NULL);
    return ctime(&now);
}
