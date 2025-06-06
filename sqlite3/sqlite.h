// sqlite.h
#ifndef SQLITE_H
#define SQLITE_H

#include <stdlib.h>
#include <sys/types.h> // For ssize_t
#include <stdint.h>
#include <stdbool.h>
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef enum
{
    EXECUTE_SUCCESS,
    EXECUTE_TABLE_FULL,
    EXECUTE_UNRECOGNIZED_STATEMENT,
} ExecuteResult;

// ============================//
#define size_of_attribute(Struct, Attribute) sizeof(((Struct *)0)->Attribute)
#define ID_SIZE size_of_attribute(Row, id)
#define USERNAME_SIZE size_of_attribute(Row, username)
#define EMAIL_SIZE size_of_attribute(Row, email)

#define ID_OFFSET 0
#define USERNAME_OFFSET (ID_OFFSET + ID_SIZE)
#define EMAIL_OFFSET (USERNAME_OFFSET + USERNAME_SIZE)
#define ROW_SIZE (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)

// ============================ //
#define PAGE_SIZE 4096 // or whatever constant you use
#define ROWS_PER_PAGE (PAGE_SIZE / ROW_SIZE)
#define TABLE_MAX_PAGES 100
#define TABLE_MAX_ROWS (ROWS_PER_PAGE * TABLE_MAX_PAGES)



typedef struct
{
    int file_descriptor;
    uint32_t file_length;
    void *pages[TABLE_MAX_PAGES];
} Pager;

typedef struct
{
    uint32_t num_rows;
    Pager *pager;
} Table;

typedef struct
{
    Table *table;
    uint32_t *row_num;
    bool end_of_table; // Indicates a position one past the last element
} Cursor;

// ============================//

typedef struct
{
    char *buffer;
    size_t buffer_length;
    ssize_t input_length;
} Buffer_input;

// ============================//
typedef enum
{
    META_COMMAND_SUCCESS,
    META_COMMAND_UNRECOGNIZED_COMMAND,
} MetaComandResult;

typedef enum
{
    PREPARE_SUCCESS,
    PREPARE_UNRECOGNIZED_STATEMENT,
    PREPARE_STRING_TOO_LONG,
    PREPARE_NEGATIVE_ID,
    PREPARE_SYNTAX_ERROR
} PrepareResult;

// ============================== //
typedef struct
{
    int32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE];

} Row;

// ============================//
typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct
{
    StatementType type;
    Row row_to_insert; // only used by insert statement
} Statement;

// ============================== //
Buffer_input *create_input_buffer();
void print_promt();
void print_row(Row *row);
void read_input(Buffer_input *input_buffer);
void close_input_buffer(Buffer_input *input_buffer);

// ================================ //
MetaComandResult do_meta_command(Buffer_input *input_buffer, Table *table);
PrepareResult prepare_insert(Buffer_input *input_buffer, Statement *statement);
PrepareResult prepare_statement(Buffer_input *input_buffer, Statement *statement);
ExecuteResult execute_statement(Statement *statement, Table *table);

void serialize_row(Row *source, void *destination);
void deserialize_row(void *source, Row *destination);
void *row_slot(Table *table, uint32_t row_num);
ExecuteResult execute_insert(Statement *statement, Table *table);
ExecuteResult execute_select(Table *table);
void *get_page(Pager *pager, uint32_t page_num);
Table *db_open(const char *filename);
void db_close(Table *table);
Pager *pager_open(const char *filename);
void pager_flush(Pager *pager, uint32_t page_num, uint32_t size);
// void free_table(Table *table);


Cursor *table_end(Table *table);
Cursor *table_start(Table *table);
void *cursor_value(Cursor *cursor);
void cursor_advance(Cursor *cursor);

#endif // SQLITE_H
