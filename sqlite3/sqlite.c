#include "sqlite.h" // use double quotes for your own headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Buffer_input *create_input_buffer()
{
    Buffer_input *buffer = malloc(sizeof(Buffer_input));
    buffer->buffer = NULL;
    buffer->input_length = 0;
    buffer->buffer_length = 0;
    return buffer;
}

void print_promt() { printf("%s", "db >"); }
void print_row(Row *row)
{
    printf("(%d, %s, %s)\n", row->id, row->username, row->email);
}
void read_input(Buffer_input *input_buffer)
{
    ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);
    if (bytes_read <= 0)
    {
        printf("%s", "Can't read input\n");
        exit(EXIT_FAILURE);
    }
    // rmove the trailing newlines
    input_buffer->buffer_length = bytes_read - 1;
    input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(Buffer_input *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}

// ==================================================== //

MetaComandResult do_meta_command(Buffer_input *input_buffer, Table *table)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        close_input_buffer(input_buffer);
        free_table(table);
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_insert(Buffer_input *input_buffer , Statement *statement){
    statement->type = STATEMENT_INSERT;
    char *key_word = strtok(input_buffer->buffer," ");
    char *id_str = strtok(NULL," ");
    char *user_name = strtok(NULL," ");
    char *user_email = strtok(NULL," ");

    if (id_str == NULL || user_name == NULL || user_email == NULL){
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_str);
    if (id < 0 ){
        return PREPARE_NEGATIVE_ID;
    }
    if (strlen(user_name)> USERNAME_SIZE){
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(user_email)> EMAIL_SIZE)
    {
        return PREPARE_STRING_TOO_LONG;
    }
    
    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username ,user_name);
    strcpy(statement->row_to_insert.email,user_email);
    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(Buffer_input *input_buffer, Statement *statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        return prepare_insert(input_buffer,statement);
    }
    if (strcmp(input_buffer->buffer, "select") == 0)
    {
        statement->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

ExecuteResult execute_statement(Statement *statement, Table *table)
{
    switch (statement->type)
    {
    case (STATEMENT_INSERT):
        return execute_insert(statement, table);
    case (STATEMENT_SELECT):
        return execute_select( table);
    default:
        return EXECUTE_UNRECOGNIZED_STATEMENT;
    }
}

// =======================================================//
void serialize_row(Row *source, void *destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    memcpy(destination + USERNAME_OFFSET, &(source->username), USERNAME_SIZE);
    memcpy(destination + EMAIL_OFFSET, &(source->email), EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destination)
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}

void *row_slot(Table *table, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = table->pages[page_num];
    if (page == NULL)
    {
        // Allocate memory only when we try to access page
        page = table->pages[page_num] = malloc(PAGE_SIZE);
    }
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

ExecuteResult execute_insert(Statement *statement, Table *table)
{
    if (table->num_rows >= TABLE_MAX_ROWS)
    {
        return EXECUTE_TABLE_FULL;
    }

    Row *row_to_insert = &(statement->row_to_insert);

    serialize_row(row_to_insert, row_slot(table, table->num_rows));
    table->num_rows += 1;

    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select( Table *table)
{
    Row row;
    for (uint32_t i = 0; i < table->num_rows; i++)
    {
        deserialize_row(row_slot(table, i), &row);
        print_row(&row);
    }
    return EXECUTE_SUCCESS;
}

Table *new_table()
{
    Table *table = (Table *)malloc(sizeof(Table));
    table->num_rows = 0;
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        table->pages[i] = NULL;
    }
    return table;
}

void free_table(Table *table)
{
    for (int i = 0; table->pages[i]; i++)
    {
        free(table->pages[i]);
    }
    free(table);
}