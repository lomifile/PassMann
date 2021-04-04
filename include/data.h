//
// Created by filip on 28. 02. 2021..
//

#ifndef PASSMANN_DATA_H
#define PASSMANN_DATA_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define DIRNAME ".passmann"
#define FILENAME "database.db"
#define TEMP ".tmp.db"

#define COLUMN_USECASE_SIZE 255
#define COLUMN_USERNAME_SIZE 255
#define COLUMN_PASSWORD_SIZE 255
#define TABLE_MAX_PAGES 100

typedef enum
{
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
} ExecuteResult;

typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LONG,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT,
    STATEMENT_SAVE_DATA,
} StatementType;

typedef struct
{
    uint32_t id;
    char usecase[COLUMN_USECASE_SIZE + 1];
    char username[COLUMN_USERNAME_SIZE + 1];
    char password[COLUMN_PASSWORD_SIZE + 1];
} Row;

typedef struct
{
    StatementType type;
    Row row_to_insert; // only used by insert statement
} Statement;

typedef struct
{
    int file_descriptor;
    uint32_t file_length;
    uint32_t num_pages;
    void *pages[TABLE_MAX_PAGES];
} Pager;

typedef struct
{
    Pager *pager;
    uint32_t root_page_num;
} Table;

typedef struct
{
    Table *table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table; // Indicates a position one past the last element
} Cursor;

typedef enum
{
    NODE_INTERNAL,
    NODE_LEAF
} NodeType;

char password[50];

#endif //PASSMANN_DATA_H
