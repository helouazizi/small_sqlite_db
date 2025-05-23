#include <stdio.h>
#include "sqlite.h"
#include <string.h>

int main(/*int argc, char *argv[]*/)
{
    Buffer_input *input_buffer = create_input_buffer();
    while (1)
    {
        print_promt();
        read_input(input_buffer);
        if (strcmp(input_buffer->buffer, ".exit") == 0)
        {
            close_input(input_buffer);
            exit(EXIT_SUCCESS);
        }
        else
            printf("Unrecognized command '%s'.\n", input_buffer->buffer);

        //  here we are checkong do is the comand is the meta command
        if (input_buffer->buffer[0] == '.')
        {
            switch (do_meta_command(input_buffer))
            {
            case (META_COMMAND_SUCCESS):
                continue;
            case (META_COMMAND_UNRECOGNIZED_COMMAND):
                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                continue;
            }
        }

        // now lets prepare our statment
        Statement statement;

        switch (prepare_stetment(input_buffer, &statement))
        {
        case PREPARE_SUCCESS:
            break;

        case PREPARE_UNRECOGNIZED_STATEMENT:
            printf("Unrecognized keyword at start of '%s'.\n",
                   input_buffer->buffer);
            continue;
        }

        // lets execute the statment
        execute_statement(&statement);
        printf("%s\n","Executed");
    }
}