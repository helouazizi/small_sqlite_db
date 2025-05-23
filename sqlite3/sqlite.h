// sqlite.h
#ifndef SQLITE_H
#define SQLITE_H

#include <stdlib.h>
#include <sys/types.h> // For ssize_t
#include <stdint.h>
#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

// ============================//
#define size_of_attribute(Struct, Attribute) sizeof(((Struct*)0)->Attribute)

#define ID_SIZE size_of_attribute(Row, id)
#define USERNAME_SIZE size_of_attribute(Row, username)
#define EMAIL_SIZE size_of_attribute(Row, email)

#define ID_OFFSET 0
#define USERNAME_OFFSET (ID_OFFSET + ID_SIZE)
#define EMAIL_OFFSET (USERNAME_OFFSET + USERNAME_SIZE)
#define ROW_SIZE (ID_SIZE + USERNAME_SIZE + EMAIL_SIZE)


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
    PREPARE_UNRECOGNIZED_STATEMENT
} PrepareResult;

// ============================//
typedef enum
{
    STATEMENT_INSERT,
    STATEMENT_SELECT
} StatementType;

typedef struct
{
    StatementType type;
    Row Row_to_insert;
} Statement;

// ============================== //
typedef struct
{
    int32_t id;
    char username[COLUMN_USERNAME_SIZE];
    char email[COLUMN_EMAIL_SIZE]

} Row;

// ============================== //
Buffer_input *create_input_buffer();
void print_promt();
void read_input(Buffer_input *input_buffer);
void close_input(Buffer_input *input_buffer);

// ================================ //
MetaComandResult do_meta_command(Buffer_input *input_buffer);
PrepareResult prepare_stetment(Buffer_input *input_buffer, Statement *stetment);
void execute_statement(Statement *statement);

#endif // SQLITE_H
