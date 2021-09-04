#include "main.h"

static void start_db(Table *tbl)
{
    printf("\n"
           "MAKE SURE TO REMEBER YOUR MASTER PASSWORD AT ALL TIMES!\n"
           "\n"
           "\n");
    if (!(strlen(password) > 0))
    {
        printf("Input master passowrd > ");
        fgets(password, sizeof password, stdin);
    }
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
            append_log(time_now(), "Exectued statement");
            break;
        case (EXECUTE_DUPLICATE_KEY):
            printf("Error: Duplicate key.\n");
            append_log(time_now(), "Error: Duplicate key.");
            break;
        case (EXECUTE_ERROR):
            printf("There was an error while trying to execute command!\n");
            append_log(time_now(), "There was an error while executing command!");
            break;
        }
    }
}

static void print_welcome()
{
    printf(
        "<<PassMann>> \n"
        "Welcome to PassMan \n"
        "Version 1.0.1 by Filip Ivanusec \n"
        "\n"
        "\n"
        "Use .help for simple tutorial on how to use PassMann \n");
}

int main()
{
    init_raw_mode();
    print_welcome();
 
    append_log(time_now(), "PassMann started");
    int file = check_db_file(FILENAME);

    if (file < 0)
    {
        printf("File doesn't exist\n"
               "Creating new database file\n");
        append_log(time_now(), "Created new db file");
    }

    Table *tbl = db_open(FILENAME);
    start_db(tbl);
}
