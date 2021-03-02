/**
 * Log
 *
 * Creates and edits data into passmannlog.txt
 * 
 :*/

#include "log.h"

void create_log_file(){
    FILE* fp = fopen(LOG_FILE , "w");
    char t_now[30];
    if(fp == NULL)
        exit(EXIT_FAILURE);

    time_t t = time(NULL);
    fprintf(fp, "%s --------> %s", INIT_MSG, ctime(&t));
    fclose(fp);
}

void display_log(){
    FILE* fp = fopen(LOG_FILE, "r");
    if(fp == NULL) {
        printf("There was a problem!");
        exit(EXIT_FAILURE);
    }
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    while((read = getline(&line,&len,fp))!=-1) {
        printf("%s", line);
    }
    fclose(fp);
    if(line)
        free(line);
}

void append_log(char*date ,char* input){
    strtok(date, "\n");
    FILE* fp = fopen(LOG_FILE, "a");
    if(fp == NULL)
        exit(EXIT_FAILURE);

    fprintf(fp, "%s --------> %s\n", date, input);
    fclose(fp);
}

bool check_log(){
    if(access(LOG_FILE, F_OK) != -1)
        return true;
    else
        return false;
}

char* time_now(){
    time_t now = time(NULL);
    return ctime(&now);
}
