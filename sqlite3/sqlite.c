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

void close_input(Buffer_input *input_buffer)
{
    free(input_buffer->buffer);
    free(input_buffer);
}

// ==================================================== //

MetaComandResult do_meta_command(Buffer_input *input_buffer)
{
    if (strcmp(input_buffer->buffer, ".exit") == 0)
    {
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_stetment(Buffer_input *input_buffer, Statement *stetment)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        stetment->type = STATEMENT_INSERT;
        return PREPARE_SUCCESS;
    }

    if (strncmp(input_buffer->buffer, "select", 6) == 0)
    {
        stetment->type = STATEMENT_SELECT;
        return PREPARE_SUCCESS;
    }

    return PREPARE_UNRECOGNIZED_STATEMENT;
}

void execute_statement(Statement *statement)
{
    switch (statement->type)
    {
    case (STATEMENT_INSERT):
        printf("This is where we would do an insert.\n");
        break;
    case (STATEMENT_SELECT):
        printf("This is where we would do a select.\n");
        break;
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