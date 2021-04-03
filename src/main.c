/**
 * Main program
 * 
 * Inputs data for either creating the user or for authenticating one
 * If the user is authenticated user is stored into session and the main database
 * is starting.
 */

#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>

#include "database.h"
#include "log.h"

bool check_dir()
{
    struct stat st = {0};
    DIR *dir = opendir(DIRNAME);
    if (dir)
    {
        closedir(dir);
        return true;
    }
    else if (ENOENT == errno)
    {
        if (stat(DIRNAME, &st) == -1)
            mkdir(DIRNAME, 0700);
        return true;
    }
    else
    {
        printf("There was an error while trying to open dir");
        append_log(time_now(), "There was an error while trying to open dir");
        exit(EXIT_FAILURE);
    }
}

void start_db(Table *tbl)
{
    InputBuffer *input_buffer = new_input_buffer();
    while (true)
    {
        print_prompt();
        read_input(input_buffer);

        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer, tbl))
            {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                continue;
            }
        }

        Statement stmt;
        switch (prepare_statement(input_buffer, &stmt, tbl))
        {
        case (PREPARE_SUCCESS):
            break;
        case (PREPARE_NEGATIVE_ID):
            printf("ID must be positive.\n");
            continue;
        case (PREPARE_STRING_TOO_LONG):
            printf("String is too long.\n");
            continue;
        case (PREPARE_SYNTAX_ERROR):
            printf("Syntax error. Could not parse statement.\n");
            continue;
        case (PREPARE_UNRECOGNIZED_STATEMENT):
            printf("Unrecognized keyword at start of '%s'.\n",
                   input_buffer->buffer);
            continue;
        }

        switch (execute_statement(&stmt, tbl))
        {
        case (EXECUTE_SUCCESS):
            printf("Executed.\n");
            break;
        case (EXECUTE_DUPLICATE_KEY):
            printf("Error: Duplicate key.\n");
            break;
        }
    }
}

void print_welcome()
{
    printf(
        "<<PassMan>> \n"
        "Welcome to PassMan \n"
        "Version 1.0 by Filip Ivanusec \n"
        "\n"
        "\n"
        "Use .help for simple tutorial on how to use PassMann \n");
}

int main()
{
    print_welcome();
    time_t now = time(NULL);
    append_log(ctime(&now), "PassMann started");

    Table *tbl = db_open(FILENAME);
    start_db(tbl);
}