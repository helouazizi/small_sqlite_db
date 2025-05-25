#include <stdio.h>
#include "sqlite.h"
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Must supply a database filename.\n");
        exit(EXIT_FAILURE);
    }

    char *filename = argv[1];
    Table *table = db_open(filename);
    Buffer_input *input_buffer = create_input_buffer();
    while (1)
    {
        print_promt();
        read_input(input_buffer);
        if (strcmp(input_buffer->buffer, ".exit") == 0)
        {
            if (input_buffer->buffer[0] == '.')
            {
                switch (do_meta_command(input_buffer, table))
                {
                case (META_COMMAND_SUCCESS):
                    continue;
                case (META_COMMAND_UNRECOGNIZED_COMMAND):
                    printf("Unrecognized command '%s'\n", input_buffer->buffer);
                    continue;
                }
            }
        }

        // now lets prepare our statment
        Statement statement;

        switch (prepare_statement(input_buffer, &statement))
        {
        case PREPARE_SUCCESS:
            break;
        case PREPARE_STRING_TOO_LONG:
            printf("String is too long.\n");
            continue;
        case PREPARE_NEGATIVE_ID:
            printf("ID must be positive.\n");
            continue;
        case PREPARE_SYNTAX_ERROR:
            printf("Syntax error. Could not parse statement.\n");
            continue;
        case PREPARE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized keyword at start of '%s'.\n",
                   input_buffer->buffer);
            continue;
        }

        // lets execute the statment
        switch (execute_statement(&statement, table))
        {
        case EXECUTE_SUCCESS:
            printf("Executed.\n");
            break;
        case EXECUTE_TABLE_FULL:
            printf(" Error: Table full.\n");
            break;
        case EXECUTE_UNRECOGNIZED_STATEMENT:
            printf(" Error: UNRECOGNIZED STATEMENT\n");
            break;
        }
    }
}