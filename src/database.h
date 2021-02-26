//
// Created by filip on 17. 02. 2021..
//

#ifndef PASSMANN_DATABASE_H
#define PASSMANN_DATABASE_H

#include <stdint.h>

#define TABLE_MAX_PAGES 100
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)
#define ROW_TABLE_HEADER printf("%10s|%10s|%10s|%10s|\n","ID","Usecase","Username","Password")

typedef struct {
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} InputBuffer;

typedef enum {
    EXECUTE_SUCCESS,
    EXECUTE_DUPLICATE_KEY,
} ExecuteResult;

typedef enum {
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum {
    PREPARE_SUCCESS,
    PREPARE_NEGATIVE_ID,
    PREPARE_STRING_TOO_LONG,
    PREPARE_SYNTAX_ERROR,
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

typedef enum {
    STATEMENT_INSERT,
    STATEMENT_SELECT,
    STATEMENT_SELECT_RAW,
    STATEMENT_SAVE_DATA,
} StatementType;

#define COLUMN_USECASE_SIZE  255
#define COLUMN_USERNAME_SIZE 255
#define COLUMN_PASSWORD_SIZE 255

typedef struct {
    uint32_t id;
    char usecase[COLUMN_USECASE_SIZE + 1];
    char username[COLUMN_USERNAME_SIZE + 1];
    char password[COLUMN_PASSWORD_SIZE + 1];
} Row;

typedef struct {
    StatementType type;
    Row row_to_insert;  // only used by insert statement
} Statement;

typedef struct {
    int file_descriptor;
    uint32_t file_length;
    uint32_t num_pages;
    void *pages[TABLE_MAX_PAGES];
} Pager;

typedef struct {
    Pager *pager;
    uint32_t root_page_num;
} Table;

typedef struct {
    Table *table;
    uint32_t page_num;
    uint32_t cell_num;
    bool end_of_table;  // Indicates a position one past the last element
} Cursor;

typedef enum {
    NODE_INTERNAL, NODE_LEAF
} NodeType;

ExecuteResult execute_statement(Statement *statement, Table *table);

ExecuteResult execute_select(Statement *statement, Table *table);

ExecuteResult execute_insert(Statement *statement, Table *table);

void leaf_node_insert(Cursor *cursor, uint32_t key, Row *value);

void leaf_node_split_and_insert(Cursor *cursor, uint32_t key, Row *value);

void update_internal_node_key(void *node, uint32_t old_key, uint32_t new_key);

void internal_node_insert(Table *table, uint32_t parent_page_num, uint32_t child_page_num);

void create_new_root(Table *table, uint32_t right_child_page_num);

uint32_t get_unused_page_num(Pager *pager);

PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *statement);

PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *statement);

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *table);

void db_close(Table *table);

void pager_flush(Pager *pager, uint32_t page_num);

void close_input_buffer(InputBuffer *input_buffer);

void read_input(InputBuffer *input_buffer);

void print_prompt();

InputBuffer *new_input_buffer();

Table *db_open(const char *filename);

Pager *pager_open(const char *filename);

void cursor_advance(Cursor *cursor);

void *cursor_value(Cursor *cursor);

Cursor *table_start(Table *table);

Cursor *table_find(Table *table, uint32_t key);

Cursor *internal_node_find(Table *table, uint32_t page_num, uint32_t key);

uint32_t internal_node_find_child(void *node, uint32_t key);

Cursor *leaf_node_find(Table *table, uint32_t page_num, uint32_t key);

void initialize_internal_node(void *node);

void initialize_leaf_node(void *node);

void deserialize_row(void *source, Row *destination);

void serialize_row(Row *source, void *destination);

void print_tree(Pager *pager, uint32_t page_num, uint32_t indentation_level);

void indent(uint32_t level);

void *get_page(Pager *pager, uint32_t page_num);

void print_constants();

uint32_t get_node_max_key(void *node);

void *leaf_node_value(void *node, uint32_t cell_num);

uint32_t *leaf_node_key(void *node, uint32_t cell_num);

void *leaf_node_cell(void *node, uint32_t cell_num);

uint32_t *leaf_node_next_leaf(void *node);

uint32_t *leaf_node_num_cells(void *node);


#endif //PASSMANN_DATABASE_H
