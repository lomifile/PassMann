/**
 * Database
 * 
 * This is main database suroce with all manipulations for storing data into database
 * Database can be saved to files and stored for safety.
 * 
 * TODO:
 *  1.Need to create function and command to retrive that file
 *  2.Need to create logging
 *
 **/

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sodium/randombytes.h>

#include "database.h"
#include "encryption.h"
#include "log.h"
#include "input.h"

const uint32_t ID_SIZE = size_of_attribute(Row, id);
const uint32_t USERNAME_SIZE = size_of_attribute(Row, username);
const uint32_t PASSWORD_SIZE = size_of_attribute(Row, password);
const uint32_t USECASE_SIZE = size_of_attribute(Row, usecase);
const uint32_t ID_OFFSET = 0;
const uint32_t USERNAME_OFFSET = ID_OFFSET + ID_SIZE;
const uint32_t PASSWORD_OFFSET = USERNAME_OFFSET + USERNAME_SIZE;
const uint32_t USECASE_OFFSET = PASSWORD_OFFSET + PASSWORD_SIZE;
const uint32_t ROW_SIZE = ID_SIZE + USECASE_SIZE +  USERNAME_SIZE + PASSWORD_SIZE;

const uint32_t PAGE_SIZE = 4096;

const uint32_t NODE_TYPE_SIZE = sizeof(uint8_t);
const uint32_t NODE_TYPE_OFFSET = 0;
const uint32_t IS_ROOT_SIZE = sizeof(uint8_t);
const uint32_t IS_ROOT_OFFSET = NODE_TYPE_SIZE;
const uint32_t PARENT_POINTER_SIZE = sizeof(uint32_t);
const uint32_t PARENT_POINTER_OFFSET = IS_ROOT_OFFSET + IS_ROOT_SIZE;
const uint8_t COMMON_NODE_HEADER_SIZE =
        NODE_TYPE_SIZE + IS_ROOT_SIZE + PARENT_POINTER_SIZE;

/*
 * Internal Node Header Layout
 */
const uint32_t INTERNAL_NODE_NUM_KEYS_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_NUM_KEYS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t INTERNAL_NODE_RIGHT_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_RIGHT_CHILD_OFFSET =
        INTERNAL_NODE_NUM_KEYS_OFFSET + INTERNAL_NODE_NUM_KEYS_SIZE;
const uint32_t INTERNAL_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                           INTERNAL_NODE_NUM_KEYS_SIZE +
                                           INTERNAL_NODE_RIGHT_CHILD_SIZE;

/*
 * Internal Node Body Layout
 */
const uint32_t INTERNAL_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CHILD_SIZE = sizeof(uint32_t);
const uint32_t INTERNAL_NODE_CELL_SIZE =
        INTERNAL_NODE_CHILD_SIZE + INTERNAL_NODE_KEY_SIZE;
/* Keep this small for testing */
const uint32_t INTERNAL_NODE_MAX_CELLS = 3;

/*
 * Leaf Node Header Layout
 */
const uint32_t LEAF_NODE_NUM_CELLS_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NUM_CELLS_OFFSET = COMMON_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_NEXT_LEAF_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_NEXT_LEAF_OFFSET =
        LEAF_NODE_NUM_CELLS_OFFSET + LEAF_NODE_NUM_CELLS_SIZE;
const uint32_t LEAF_NODE_HEADER_SIZE = COMMON_NODE_HEADER_SIZE +
                                       LEAF_NODE_NUM_CELLS_SIZE +
                                       LEAF_NODE_NEXT_LEAF_SIZE;

/*
 * Leaf Node Body Layout
 */
const uint32_t LEAF_NODE_KEY_SIZE = sizeof(uint32_t);
const uint32_t LEAF_NODE_KEY_OFFSET = 0;
const uint32_t LEAF_NODE_VALUE_SIZE = ROW_SIZE;
//const uint32_t LEAF_NODE_VALUE_OFFSET =
//        LEAF_NODE_KEY_OFFSET + LEAF_NODE_KEY_SIZE;
const uint32_t LEAF_NODE_CELL_SIZE = LEAF_NODE_KEY_SIZE + LEAF_NODE_VALUE_SIZE;
const uint32_t LEAF_NODE_SPACE_FOR_CELLS = PAGE_SIZE - LEAF_NODE_HEADER_SIZE;
const uint32_t LEAF_NODE_MAX_CELLS =
        LEAF_NODE_SPACE_FOR_CELLS / LEAF_NODE_CELL_SIZE;
const uint32_t LEAF_NODE_RIGHT_SPLIT_COUNT = (LEAF_NODE_MAX_CELLS + 1) / 2;
const uint32_t LEAF_NODE_LEFT_SPLIT_COUNT =
        (LEAF_NODE_MAX_CELLS + 1) - LEAF_NODE_RIGHT_SPLIT_COUNT;

void print_row_encrypted(Row *selected_row) {
    printf("============================================\n");
    printf("%10d|%10s|%10s|%10s|\n", selected_row->id, selected_row->usecase, selected_row->username, selected_row->password);
}

void print_row(Row *selected_row) {
    char *dec_password = decrypt_data(selected_row->password);
    printf("============================================\n");
    printf("%10d|%10s|%10s|%10s|\n", selected_row->id, selected_row->usecase, selected_row->username, dec_password);
}

NodeType get_node_type(void *node) {
    uint8_t value = *((uint8_t *) (node + NODE_TYPE_OFFSET));
    return (NodeType) value;
}

void set_node_type(void *node, NodeType type) {
    uint8_t value = type;
    *((uint8_t *) (node + NODE_TYPE_OFFSET)) = value;
}

bool is_node_root(void *node) {
    uint8_t value = *((uint8_t *) (node + IS_ROOT_OFFSET));
    return (bool) value;
}

void set_node_root(void *node, bool is_root) {
    uint8_t value = is_root;
    *((uint8_t *) (node + IS_ROOT_OFFSET)) = value;
}

uint32_t *node_parent(void *node) { return node + PARENT_POINTER_OFFSET; }

uint32_t *internal_node_num_keys(void *node) {
    return node + INTERNAL_NODE_NUM_KEYS_OFFSET;
}

uint32_t *internal_node_right_child(void *node) {
    return node + INTERNAL_NODE_RIGHT_CHILD_OFFSET;
}

uint32_t *internal_node_cell(void *node, uint32_t cell_num) {
    return node + INTERNAL_NODE_HEADER_SIZE + cell_num * INTERNAL_NODE_CELL_SIZE;
}

uint32_t *internal_node_child(void *node, uint32_t child_num) {
    uint32_t num_keys = *internal_node_num_keys(node);
    if (child_num > num_keys) {
        printf("Tried to access child_num %d > num_keys %d\n", child_num, num_keys);
        exit(EXIT_FAILURE);
    } else if (child_num == num_keys) {
        return internal_node_right_child(node);
    } else {
        return internal_node_cell(node, child_num);
    }
}

uint32_t *internal_node_key(void *node, uint32_t key_num) {
    return (void *) internal_node_cell(node, key_num) + INTERNAL_NODE_CHILD_SIZE;
}

uint32_t *leaf_node_num_cells(void *node) {
    return node + LEAF_NODE_NUM_CELLS_OFFSET;
}

uint32_t *leaf_node_next_leaf(void *node) {
    return node + LEAF_NODE_NEXT_LEAF_OFFSET;
}

void *leaf_node_cell(void *node, uint32_t cell_num) {
    return node + LEAF_NODE_HEADER_SIZE + cell_num * LEAF_NODE_CELL_SIZE;
}

uint32_t *leaf_node_key(void *node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num);
}

void *leaf_node_value(void *node, uint32_t cell_num) {
    return leaf_node_cell(node, cell_num) + LEAF_NODE_KEY_SIZE;
}

uint32_t get_node_max_key(void *node) {
    switch (get_node_type(node)) {
        case NODE_INTERNAL:
            return *internal_node_key(node, *internal_node_num_keys(node) - 1);
        case NODE_LEAF:
            return *leaf_node_key(node, *leaf_node_num_cells(node) - 1);
    }
}

void print_constants() {
    printf("ROW_SIZE: %d\n", ROW_SIZE);
    printf("COMMON_NODE_HEADER_SIZE: %d\n", COMMON_NODE_HEADER_SIZE);
    printf("LEAF_NODE_HEADER_SIZE: %d\n", LEAF_NODE_HEADER_SIZE);
    printf("LEAF_NODE_CELL_SIZE: %d\n", LEAF_NODE_CELL_SIZE);
    printf("LEAF_NODE_SPACE_FOR_CELLS: %d\n", LEAF_NODE_SPACE_FOR_CELLS);
    printf("LEAF_NODE_MAX_CELLS: %d\n", LEAF_NODE_MAX_CELLS);
}

void *get_page(Pager *ptr, uint32_t page_num) {
    if (page_num > TABLE_MAX_PAGES) {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_num,
               TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (ptr->pages[page_num] == NULL) {
        // Cache miss. Allocate memory and load from file.
        void *page = malloc(PAGE_SIZE);
        uint32_t num_pages = ptr->file_length / PAGE_SIZE;

        // We might save a partial page at the end of the file
        if (ptr->file_length % PAGE_SIZE) {
            num_pages += 1;
        }

        if (page_num <= num_pages) {
            lseek(ptr->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(ptr->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1) {
                printf("Error reading file: %d\n", errno);
                exit(EXIT_FAILURE);
            }
        }

        ptr->pages[page_num] = page;

        if (page_num >= ptr->num_pages) {
            ptr->num_pages = page_num + 1;
        }
    }

    return ptr->pages[page_num];
}

void indent(uint32_t level) {
    for (uint32_t i = 0; i < level; i++) {
        printf("  ");
    }
}

void print_tree(Pager *ptr, uint32_t page_num, uint32_t indentation_level) {
    void *node = get_page(ptr, page_num);
    uint32_t num_keys, child;

    switch (get_node_type(node)) {
        case (NODE_LEAF):
            num_keys = *leaf_node_num_cells(node);
            indent(indentation_level);
            printf("- leaf (size %d)\n", num_keys);
            for (uint32_t i = 0; i < num_keys; i++) {
                indent(indentation_level + 1);
                printf("- %d\n", *leaf_node_key(node, i));
            }
            break;
        case (NODE_INTERNAL):
            num_keys = *internal_node_num_keys(node);
            indent(indentation_level);
            printf("- internal (size %d)\n", num_keys);
            for (uint32_t i = 0; i < num_keys; i++) {
                child = *internal_node_child(node, i);
                print_tree(ptr, child, indentation_level + 1);

                indent(indentation_level + 1);
                printf("- key %d\n", *internal_node_key(node, i));
            }
            child = *internal_node_right_child(node);
            print_tree(ptr, child, indentation_level + 1);
            break;
    }
}

void serialize_row(Row *source, void *destination) {
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USECASE_OFFSET, &(source->usecase), USECASE_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + PASSWORD_OFFSET, &(source->password), PASSWORD_SIZE);
}

void deserialize_row(void *source, Row *destination) {
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->usecase), source + USECASE_OFFSET, USECASE_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->password), source + PASSWORD_OFFSET, PASSWORD_SIZE);
}

void initialize_leaf_node(void *node) {
    set_node_type(node, NODE_LEAF);
    set_node_root(node, false);
    *leaf_node_num_cells(node) = 0;
    *leaf_node_next_leaf(node) = 0;  // 0 represents no sibling
}

void initialize_internal_node(void *node) {
    set_node_type(node, NODE_INTERNAL);
    set_node_root(node, false);
    *internal_node_num_keys(node) = 0;
}

Cursor *leaf_node_find(Table *tbl, uint32_t page_num, uint32_t i) {
    void *node = get_page(tbl->pager, page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    Cursor *ptr = malloc(sizeof(Cursor));
    ptr->table = tbl;
    ptr->page_num = page_num;
    ptr->end_of_table = false;

    // Binary search
    uint32_t min_index = 0;
    uint32_t one_past_max_index = num_cells;
    while (one_past_max_index != min_index) {
        uint32_t index = (min_index + one_past_max_index) / 2;
        uint32_t key_at_index = *leaf_node_key(node, index);
        if (i == key_at_index) {
            ptr->cell_num = index;
            return ptr;
        }
        if (i < key_at_index) {
            one_past_max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    ptr->cell_num = min_index;
    return ptr;
}

uint32_t internal_node_find_child(void *node, uint32_t key_value) {
    /*
    Return the index of the child which should contain
    the given key_value.
    */

    uint32_t num_keys = *internal_node_num_keys(node);

    /* Binary search */
    uint32_t min_index = 0;
    uint32_t max_index = num_keys; /* there is one more child than key_value */

    while (min_index != max_index) {
        uint32_t index = (min_index + max_index) / 2;
        uint32_t key_to_right = *internal_node_key(node, index);
        if (key_to_right >= key_value) {
            max_index = index;
        } else {
            min_index = index + 1;
        }
    }

    return min_index;
}

Cursor *internal_node_find(Table *tbl, uint32_t page_num, uint32_t keyValue) {
    void *node = get_page(tbl->pager, page_num);

    uint32_t child_index = internal_node_find_child(node, keyValue);
    uint32_t child_num = *internal_node_child(node, child_index);
    void *child = get_page(tbl->pager, child_num);
    switch (get_node_type(child)) {
        case NODE_LEAF:
            return leaf_node_find(tbl, child_num, keyValue);
        case NODE_INTERNAL:
            return internal_node_find(tbl, child_num, keyValue);
    }
}

/*
Return the position of the given keyValue.
If the keyValue is not present, return the position
where it should be inserted
*/
Cursor *table_find(Table *tbl, uint32_t keyValue) {
    uint32_t root_page_num = tbl->root_page_num;
    void *root_node = get_page(tbl->pager, root_page_num);

    if (get_node_type(root_node) == NODE_LEAF) {
        return leaf_node_find(tbl, root_page_num, keyValue);
    } else {
        return internal_node_find(tbl, root_page_num, keyValue);
    }
}

Cursor *table_start(Table *tbl) {
    Cursor *ptr = table_find(tbl, 0);

    void *node = get_page(tbl->pager, ptr->page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);
    ptr->end_of_table = (num_cells == 0);

    return ptr;
}

void *cursor_value(Cursor *ptr) {
    uint32_t page_num = ptr->page_num;
    void *page = get_page(ptr->table->pager, page_num);
    return leaf_node_value(page, ptr->cell_num);
}

void cursor_advance(Cursor *ptr) {
    uint32_t page_num = ptr->page_num;
    void *node = get_page(ptr->table->pager, page_num);

    ptr->cell_num += 1;
    if (ptr->cell_num >= (*leaf_node_num_cells(node))) {
        /* Advance to next leaf node */
        uint32_t next_page_num = *leaf_node_next_leaf(node);
        if (next_page_num == 0) {
            /* This was rightmost leaf */
            ptr->end_of_table = true;
        } else {
            ptr->page_num = next_page_num;
            ptr->cell_num = 0;
        }
    }
}

Pager *pager_open(const char *filename) {
    int fd = open(filename,
                  O_RDWR |      // Read/Write mode
                  O_CREAT,  // Create file if it does not exist
                  S_IWUSR |     // User write permission
                  S_IRUSR   // User read permission
    );

    if (fd == -1) {
        printf("Unable to open file\n");
        exit(EXIT_FAILURE);
    }

    off_t file_length = lseek(fd, 0, SEEK_END);

    Pager *ptr = malloc(sizeof(Pager));
    ptr->file_descriptor = fd;
    ptr->file_length = file_length;
    ptr->num_pages = (file_length / PAGE_SIZE);

    if (file_length % PAGE_SIZE != 0) {
        printf("Db file is not a whole number of pages. Corrupt file.\n");
        exit(EXIT_FAILURE);
    }

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        ptr->pages[i] = NULL;
    }

    return ptr;
}

Table *db_open(const char *filename) {
    Pager *ptr = pager_open(filename);

    Table *tbl = malloc(sizeof(Table));
    tbl->pager = ptr;
    tbl->root_page_num = 0;

    if (ptr->num_pages == 0) {
        // New database file. Initialize page 0 as leaf node.
        void *root_node = get_page(ptr, 0);
        initialize_leaf_node(root_node);
        set_node_root(root_node, true);
    }

    printf("Table loaded! \n");

    return tbl;
}

InputBuffer *new_input_buffer() {
    InputBuffer *input_buffer = malloc(sizeof(InputBuffer));
    input_buffer->buffer = NULL;
    input_buffer->buffer_length = 0;
    input_buffer->input_length = 0;

    return input_buffer;
}

void print_prompt() { printf("passman> "); }

void print_help() {
    printf("How to use PassMann?\n"
           "Here are all of your commands \n"
           "Meta commands: \n"
           ".btree -> Shows you the B-tree structure of your database\n"
           ".constants -> Shows you the constants\n"
           ".exit -> Quits the program and flushes the database\n"
           ".passgen -> Generates password if you want one\n"
           ".log -> Shows you the log of usage"
           "\n"
           "Data handling:\n"
           "insert <ID> <USECASE> <USERNAME> <PASSWORD> -> Stores data into the system\n"
           "select -> Shows you your stored data into system\n"
           "raw -> Show you data with encryption\n"
           "save -> Flushes and reloads database\n");
}

void read_input(InputBuffer *input_buffer) {
    ssize_t bytes_read =
            getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

    if (bytes_read <= 0) {
        printf("Error reading input\n");
        exit(EXIT_FAILURE);
    }

    // Ignore trailing newline
    input_buffer->input_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
    fflush(stdin);
}

void close_input_buffer(InputBuffer *input_buffer) {
    free(input_buffer->buffer);
    free(input_buffer);
}

void pager_flush(Pager *ptr, uint32_t page_num) {
    if (ptr->pages[page_num] == NULL) {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(ptr->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

    if (offset == -1) {
        printf("Error seeking: %d\n", errno);
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written =
            write(ptr->file_descriptor, ptr->pages[page_num], PAGE_SIZE);

    if (bytes_written == -1) {
        printf("Error writing: %d\n", errno);
        exit(EXIT_FAILURE);
    }
}

void db_close(Table *tbl) {
    Pager *ptr = tbl->pager;

    for (uint32_t i = 0; i < ptr->num_pages; i++) {
        if (ptr->pages[i] == NULL) {
            continue;
        }
        pager_flush(ptr, i);
        free(ptr->pages[i]);
        ptr->pages[i] = NULL;
    }

    int result = close(ptr->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = ptr->pages[i];
        if (page) {
            free(page);
            ptr->pages[i] = NULL;
        }
    }
    free(ptr);
    free(tbl);
}

void randomPasswordGeneration(int N) {
    // Initialize counter
    int i;

    uint32_t randomizer;

    // Array of numbers
    char numbers[] = "0123456789";

    // Array of small alphabets
    char letter[] = "abcdefghijklmnoqprstuvwyzx";

    // Array of capital alphabets
    char LETTER[] = "ABCDEFGHIJKLMNOQPRSTUYWVZX";

    // Array of all the special symbols
    char symbols[] = "!@#$^&*?";

    // Stores the random password
    char password[N];

    // To select the randomizer
    // inside the loop
    randomizer = randombytes_random() % 4;

    // Iterate over the range [0, N]
    for (i = 0; i < N; i++) {

        if (randomizer == 1) {
            password[i] = numbers[randombytes_uniform(sizeof(numbers)) % 10];
            randomizer = randombytes_random() % 4;
            printf("%c", password[i]);
        } else if (randomizer == 2) {
            password[i] = symbols[randombytes_uniform(sizeof(symbols)) % 8];
            randomizer = randombytes_random() % 4;
            printf("%c", password[i]);
        } else if (randomizer == 3) {
            password[i] = LETTER[randombytes_uniform(sizeof(LETTER)) % 26];
            randomizer = randombytes_random() % 4;
            printf("%c", password[i]);
        } else {
            password[i] = letter[randombytes_uniform(sizeof(letter)) % 26];
            randomizer = randombytes_random() % 4;
            printf("%c", password[i]);
        }
    }
    printf("\n");
}

int get_last_id(Table *tbl){
    Cursor *ptr = table_start(tbl);
    int last_id = 0;
    Row check_row;
    while (!(ptr->end_of_table)) {
        deserialize_row(cursor_value(ptr), &check_row);
        cursor_advance(ptr);
    }
    if(ptr->end_of_table){
        last_id = check_row.id;
    }
    free(ptr);
    printf("Last id is: %d\n", last_id);
    if(last_id == 1){
        return 0;
    }
    return last_id;
}

MetaCommandResult do_meta_command(InputBuffer *input_buffer, Table *tbl) {
    if (strcmp(input_buffer->buffer, ".exit") == 0) {
        close_input_buffer(input_buffer);
        db_close(tbl);
        append_log(time_now(), "PassMan exit");
        exit(EXIT_SUCCESS);
    } else if (strcmp(input_buffer->buffer, ".btree") == 0) {
        printf("Tree:\n");
        print_tree(tbl->pager, 0, 0);
        append_log(time_now(), "Show tree");
        return META_COMMAND_SUCCESS;
    } else if (strcmp(input_buffer->buffer, ".constants") == 0) {
        printf("Constants:\n");
        print_constants();
        append_log(time_now(), "Print constants");
        return META_COMMAND_SUCCESS;
    } else if (strcmp(input_buffer->buffer, ".passgen") == 0) {
        int length = 0, input_result;
        do{
            printf("Input length of password> ");
            input_result = int_input(&length);
            if (length == 0){
                printf("Nothing was generated!\n");
                return META_COMMAND_SUCCESS;
            }
        }while(input_result != 1);
        randomPasswordGeneration(length);
        append_log(time_now(), "Generated password");
        return META_COMMAND_SUCCESS;
    } else if(strcmp(input_buffer->buffer, ".log") == 0){
        display_log();
        append_log(time_now(), "Displayed log");
        return META_COMMAND_SUCCESS;
    } else if (strcmp(input_buffer->buffer, ".help") == 0) {
        print_help();
        append_log(time_now(), "Requested help");
        return META_COMMAND_SUCCESS;
    } else if(strcmp(input_buffer->buffer, ".lastid") == 0){
        get_last_id(tbl);
        return META_COMMAND_SUCCESS;
    } else {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_insert(InputBuffer *input_buffer, Statement *stmt, Table *tbl) {
    stmt->type = STATEMENT_INSERT;
    int id;
    char *keyword = strtok(input_buffer->buffer, " ");
//    char *id_string = strtok(NULL, " ");
    char *usecase = strtok(NULL, " ");
    char *username = strtok(NULL, " ");
    char *password = strtok(NULL, " ");

    if (username == NULL || password == NULL || usecase == NULL) {
        return PREPARE_SYNTAX_ERROR;
    }

    id = get_last_id(tbl) + 1;
    if (id < 0) {
        return PREPARE_NEGATIVE_ID;
    }
    if (strlen(username) > COLUMN_USERNAME_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(password) > COLUMN_PASSWORD_SIZE) {
        return PREPARE_STRING_TOO_LONG;
    }

    stmt->row_to_insert.id = id;
    strcpy(stmt->row_to_insert.usecase, usecase);
    strcpy(stmt->row_to_insert.username, username);
    strcpy(stmt->row_to_insert.password, password);

    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(InputBuffer *input_buffer,
                                Statement *stmt, Table* tbl) {
    if (strncmp(input_buffer->buffer, "insert", 6) == 0) {
        append_log(time_now(), "Insert new data");
        return prepare_insert(input_buffer, stmt, tbl);
    }
    if (strcmp(input_buffer->buffer, "raw") == 0) {
        stmt->type = STATEMENT_SELECT_RAW;
        append_log(time_now(), "Raw select");
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "select") == 0) {
        stmt->type = STATEMENT_SELECT;
        append_log(time_now(), "Selected data");
        return PREPARE_SUCCESS;
    }
    if (strcmp(input_buffer->buffer, "save") == 0) {
        stmt->type = STATEMENT_SAVE_DATA;
        append_log(time_now(), "Saved data");
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

/*
Until we start recycling free pages, new pages will always
go onto the end of the database file
*/
uint32_t get_unused_page_num(Pager *ptr) { return ptr->num_pages; }

void create_new_root(Table *tbl, uint32_t right_child_page_num) {
    /*
    Handle splitting the root.
    Old root copied to new page, becomes left child.
    Address of right child passed in.
    Re-initialize root page to contain the new root node.
    New root node points to two children.
    */

    void *root = get_page(tbl->pager, tbl->root_page_num);
    void *right_child = get_page(tbl->pager, right_child_page_num);
    uint32_t left_child_page_num = get_unused_page_num(tbl->pager);
    void *left_child = get_page(tbl->pager, left_child_page_num);

    /* Left child has data copied from old root */
    memcpy(left_child, root, PAGE_SIZE);
    set_node_root(left_child, false);

    /* Root node is a new internal node with one key and two children */
    initialize_internal_node(root);
    set_node_root(root, true);
    *internal_node_num_keys(root) = 1;
    *internal_node_child(root, 0) = left_child_page_num;
    uint32_t left_child_max_key = get_node_max_key(left_child);
    *internal_node_key(root, 0) = left_child_max_key;
    *internal_node_right_child(root) = right_child_page_num;
    *node_parent(left_child) = tbl->root_page_num;
    *node_parent(right_child) = tbl->root_page_num;
}

void internal_node_insert(Table *tbl, uint32_t parent_page_num,
                          uint32_t child_page_num) {
    /*
    Add a new child/key pair to parent that corresponds to child
    */

    void *parent = get_page(tbl->pager, parent_page_num);
    void *child = get_page(tbl->pager, child_page_num);
    uint32_t child_max_key = get_node_max_key(child);
    uint32_t index = internal_node_find_child(parent, child_max_key);

    uint32_t original_num_keys = *internal_node_num_keys(parent);
    *internal_node_num_keys(parent) = original_num_keys + 1;

    if (original_num_keys >= INTERNAL_NODE_MAX_CELLS) {
        printf("Need to implement splitting internal node\n");
        exit(EXIT_FAILURE);
    }

    uint32_t right_child_page_num = *internal_node_right_child(parent);
    void *right_child = get_page(tbl->pager, right_child_page_num);

    if (child_max_key > get_node_max_key(right_child)) {
        /* Replace right child */
        *internal_node_child(parent, original_num_keys) = right_child_page_num;
        *internal_node_key(parent, original_num_keys) =
                get_node_max_key(right_child);
        *internal_node_right_child(parent) = child_page_num;
    } else {
        /* Make room for the new cell */
        for (uint32_t i = original_num_keys; i > index; i--) {
            void *destination = internal_node_cell(parent, i);
            void *source = internal_node_cell(parent, i - 1);
            memcpy(destination, source, INTERNAL_NODE_CELL_SIZE);
        }
        *internal_node_child(parent, index) = child_page_num;
        *internal_node_key(parent, index) = child_max_key;
    }
}

void update_internal_node_key(void *node, uint32_t old_key, uint32_t new_key) {
    uint32_t old_child_index = internal_node_find_child(node, old_key);
    *internal_node_key(node, old_child_index) = new_key;
}

void leaf_node_split_and_insert(Cursor *ptr, uint32_t keyValue, Row *value) {
    /*
    Create a new node and move half the cells over.
    Insert the new value in one of the two nodes.
    Update parent or create a new parent.
    */

    void *old_node = get_page(ptr->table->pager, ptr->page_num);
    uint32_t old_max = get_node_max_key(old_node);
    uint32_t new_page_num = get_unused_page_num(ptr->table->pager);
    void *new_node = get_page(ptr->table->pager, new_page_num);
    initialize_leaf_node(new_node);
    *node_parent(new_node) = *node_parent(old_node);
    *leaf_node_next_leaf(new_node) = *leaf_node_next_leaf(old_node);
    *leaf_node_next_leaf(old_node) = new_page_num;

    /*
    All existing keys plus new keyValue should should be divided
    evenly between old (left) and new (right) nodes.
    Starting from the right, move each keyValue to correct position.
    */
    for (int32_t i = LEAF_NODE_MAX_CELLS; i >= 0; i--) {
        void *destination_node;
        if (i >= LEAF_NODE_LEFT_SPLIT_COUNT) {
            destination_node = new_node;
        } else {
            destination_node = old_node;
        }
        uint32_t index_within_node = i % LEAF_NODE_LEFT_SPLIT_COUNT;
        void *destination = leaf_node_cell(destination_node, index_within_node);

        if (i == ptr->cell_num) {
            serialize_row(value,
                          leaf_node_value(destination_node, index_within_node));
            *leaf_node_key(destination_node, index_within_node) = keyValue;
        } else if (i > ptr->cell_num) {
            memcpy(destination, leaf_node_cell(old_node, i - 1), LEAF_NODE_CELL_SIZE);
        } else {
            memcpy(destination, leaf_node_cell(old_node, i), LEAF_NODE_CELL_SIZE);
        }
    }

    /* Update cell count on both leaf nodes */
    *(leaf_node_num_cells(old_node)) = LEAF_NODE_LEFT_SPLIT_COUNT;
    *(leaf_node_num_cells(new_node)) = LEAF_NODE_RIGHT_SPLIT_COUNT;

    if (is_node_root(old_node)) {
        return create_new_root(ptr->table, new_page_num);
    } else {
        uint32_t parent_page_num = *node_parent(old_node);
        uint32_t new_max = get_node_max_key(old_node);
        void *parent = get_page(ptr->table->pager, parent_page_num);

        update_internal_node_key(parent, old_max, new_max);
        internal_node_insert(ptr->table, parent_page_num, new_page_num);
        return;
    }
}

void leaf_node_insert(Cursor *ptr, uint32_t keyValue, Row *value) {
    void *node = get_page(ptr->table->pager, ptr->page_num);

    uint32_t num_cells = *leaf_node_num_cells(node);
    if (num_cells >= LEAF_NODE_MAX_CELLS) {
        // Node full
        leaf_node_split_and_insert(ptr, keyValue, value);
        return;
    }

    if (ptr->cell_num < num_cells) {
        // Make room for new cell
        for (uint32_t i = num_cells; i > ptr->cell_num; i--) {
            memcpy(leaf_node_cell(node, i), leaf_node_cell(node, i - 1),
                   LEAF_NODE_CELL_SIZE);
        }
    }

    *(leaf_node_num_cells(node)) += 1;
    *(leaf_node_key(node, ptr->cell_num)) = keyValue;
    serialize_row(value, leaf_node_value(node, ptr->cell_num));
}

ExecuteResult execute_insert(Statement *stmt, Table *tbl) {
    Row *row_to_insert = &(stmt->row_to_insert);
    uint32_t key_to_insert = row_to_insert->id;
    Cursor *ptr = table_find(tbl, key_to_insert);

    void *node = get_page(tbl->pager, ptr->page_num);
    uint32_t num_cells = *leaf_node_num_cells(node);

    if (ptr->cell_num < num_cells) {
        uint32_t key_at_index = *leaf_node_key(node, ptr->cell_num);
        if (key_at_index == key_to_insert) {
            return EXECUTE_DUPLICATE_KEY;
        }
    }

    leaf_node_insert(ptr, row_to_insert->id, row_to_insert);

    free(ptr);

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_raw_select(Table *tbl) {
    Cursor *ptr = table_start(tbl);

    ROW_TABLE_HEADER;
    Row rowData;
    while (!(ptr->end_of_table)) {
        deserialize_row(cursor_value(ptr), &rowData);
        print_row_encrypted(&rowData);
        cursor_advance(ptr);
    }

    free(ptr);
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Table *tbl) {
    Cursor *ptr = table_start(tbl);

    ROW_TABLE_HEADER;
    Row rowData;
    while (!(ptr->end_of_table)) {
        deserialize_row(cursor_value(ptr), &rowData);
        print_row(&rowData);
        cursor_advance(ptr);
    }
    free(ptr);

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_save_data(Table *tbl) {
    Pager *ptr = tbl->pager;

    for (uint32_t i = 0; i < ptr->num_pages; i++) {
        if (ptr->pages[i] == NULL) {
            continue;
        }
        pager_flush(ptr, i);
        free(ptr->pages[i]);
        ptr->pages[i] = NULL;
    }

    int result = close(ptr->file_descriptor);
    if (result == -1) {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++) {
        void *page = ptr->pages[i];
        if (page) {
            free(page);
            ptr->pages[i] = NULL;
        }
    }
    free(ptr);
    free(tbl);

    ptr = pager_open(FILENAME);

    tbl = malloc(sizeof(Table));
    tbl->pager = ptr;
    tbl->root_page_num = 0;

    if (ptr->num_pages == 0) {
        // New database file. Initialize page 0 as leaf node.
        void *root_node = get_page(ptr, 0);
        initialize_leaf_node(root_node);
        set_node_root(root_node, true);
    }

    printf("Table data saved! \n");

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_statement(Statement *stmt, Table *tbl) {
    switch (stmt->type) {
        case (STATEMENT_INSERT):
            return execute_insert(stmt, tbl);
        case (STATEMENT_SELECT):
            return execute_select(tbl);
        case (STATEMENT_SELECT_RAW):
            return execute_raw_select(tbl);
        case (STATEMENT_SAVE_DATA):
            return execute_save_data(tbl);
    }
}