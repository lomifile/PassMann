//
// Created by filip on 17. 02. 2021..
//

#ifndef PASSMANN_DATABASE_H
#define PASSMANN_DATABASE_H

#include <include/data.h>
#include "input.h"

#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)
#define ROW_TABLE_HEADER printf("%10s|%10s|%10s|%20s|\n", "ID", "Usecase", "Username", "Password")

extern InputBuffer Input_Buffer;
extern ExecuteResult Execute_Result;
extern MetaCommandResult Meta_Command_Result;
extern PrepareResult Prepare_Result;
extern StatementType Statement_Type;
extern Row row;
extern Statement statement;
extern Pager pager;
extern Table table;
extern Cursor cursor;
extern NodeType Node_Type;

ExecuteResult execute_statement(Statement *stmt, Table *tbl);

ExecuteResult execute_insert(Statement *stmt, Table *tbl);

void leaf_node_insert(Cursor *ptr, uint32_t keyValue, Row *value);

void leaf_node_split_and_insert(Cursor *ptr, uint32_t keyValue, Row *value);

void update_internal_node_key(void *node, uint32_t old_key, uint32_t new_key);

void internal_node_insert(Table *tbl, uint32_t parent_page_num, uint32_t child_page_num);

void create_new_root(Table *tbl, uint32_t right_child_page_num);

uint32_t get_unused_page_num(Pager *ptr);

PrepareResult prepare_statement(InputBuffer *input_buffer, Statement *stmt, Table *tbl);

PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *stmt, Table *tbl);

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *tbl);

void db_close(Table *tbl);

void pager_flush(Pager *ptr, uint32_t page_num);

void close_input_buffer(InputBuffer *input_buffer);

void read_input(InputBuffer *input_buffer);

void print_prompt();

InputBuffer *new_input_buffer();

Table *db_open(const char *filename);

Pager *pager_open(const char *filename);

void cursor_advance(Cursor *ptr);

void *cursor_value(Cursor *ptr);

Cursor *table_start(Table *tbl);

Cursor *table_find(Table *tbl, uint32_t keyValue);

Cursor *internal_node_find(Table *tbl, uint32_t page_num, uint32_t keyValue);

uint32_t internal_node_find_child(void *node, uint32_t key_value);

Cursor *leaf_node_find(Table *tbl, uint32_t page_num, uint32_t i);

void initialize_internal_node(void *node);

void initialize_leaf_node(void *node);

void deserialize_row(void *source, Row *destination);

void serialize_row(Row *source, void *destination);

void print_tree(Pager *ptr, uint32_t page_num, uint32_t indentation_level);

void indent(uint32_t level);

void *get_page(Pager *ptr, uint32_t page_num);

void print_constants();

uint32_t get_node_max_key(void *node);

void *leaf_node_value(void *node, uint32_t cell_num);

uint32_t *leaf_node_key(void *node, uint32_t cell_num);

void *leaf_node_cell(void *node, uint32_t cell_num);

uint32_t *leaf_node_next_leaf(void *node);

uint32_t *leaf_node_num_cells(void *node);

#endif //PASSMANN_DATABASE_H
