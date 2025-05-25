#include "sqlite.h" // use double quotes for your own headers
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>

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
        db_close(table);
        exit(EXIT_SUCCESS);
    }
    else
    {
        return META_COMMAND_UNRECOGNIZED_COMMAND;
    }
}

PrepareResult prepare_insert(Buffer_input *input_buffer, Statement *statement)
{
    statement->type = STATEMENT_INSERT;
    char *key_word = strtok(input_buffer->buffer, " ");
    char *id_str = strtok(NULL, " ");
    char *user_name = strtok(NULL, " ");
    char *user_email = strtok(NULL, " ");

    if (id_str == NULL || user_name == NULL || user_email == NULL)
    {
        return PREPARE_SYNTAX_ERROR;
    }

    int id = atoi(id_str);
    if (id < 0)
    {
        return PREPARE_NEGATIVE_ID;
    }
    if (strlen(user_name) > USERNAME_SIZE)
    {
        return PREPARE_STRING_TOO_LONG;
    }
    if (strlen(user_email) > EMAIL_SIZE)
    {
        return PREPARE_STRING_TOO_LONG;
    }

    statement->row_to_insert.id = id;
    strcpy(statement->row_to_insert.username, user_name);
    strcpy(statement->row_to_insert.email, user_email);
    return PREPARE_SUCCESS;
}

PrepareResult prepare_statement(Buffer_input *input_buffer, Statement *statement)
{
    if (strncmp(input_buffer->buffer, "insert", 6) == 0)
    {
        return prepare_insert(input_buffer, statement);
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
        return execute_select(table);
    default:
        return EXECUTE_UNRECOGNIZED_STATEMENT;
    }
}

// =======================================================//
void serialize_row(Row *source, void *destination)
{
    memcpy(destination + ID_OFFSET, &(source->id), ID_SIZE);
    strncpy(destination + USERNAME_OFFSET, source->username, USERNAME_SIZE);
    strncpy(destination + EMAIL_OFFSET, source->email, EMAIL_SIZE);
}

void deserialize_row(void *source, Row *destination)
{
    memcpy(&(destination->id), source + ID_OFFSET, ID_SIZE);
    memcpy(&(destination->username), source + USERNAME_OFFSET, USERNAME_SIZE);
    memcpy(&(destination->email), source + EMAIL_OFFSET, EMAIL_SIZE);
}
void *get_page(Pager *pager, uint32_t page_num)
{
    if (page_num > TABLE_MAX_PAGES)
    {
        printf("Tried to fetch page number out of bounds. %d > %d\n", page_num,
               TABLE_MAX_PAGES);
        exit(EXIT_FAILURE);
    }

    if (pager->pages[page_num] == NULL)
    {
        // Cache miss. Allocate memory and load from file.
        void *page = malloc(PAGE_SIZE);
        uint32_t num_pages = pager->file_length / PAGE_SIZE;

        // We might save a partial page at the end of the file
        if (pager->file_length % PAGE_SIZE)
        {
            num_pages += 1;
        }

        if (page_num <= num_pages)
        {
            lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);
            ssize_t bytes_read = read(pager->file_descriptor, page, PAGE_SIZE);
            if (bytes_read == -1)
            {
                printf("Error reading file \n");
                exit(EXIT_FAILURE);
            }
        }

        pager->pages[page_num] = page;
    }

    return pager->pages[page_num];
}
void *row_slot(Table *table, uint32_t row_num)
{
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = get_page(table->pager, page_num);
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

ExecuteResult execute_insert(Statement *statement, Table *table)
{
    if (table->num_rows >= TABLE_MAX_ROWS) {
        return EXECUTE_TABLE_FULL;
    }

    Row* row_to_insert = &(statement->row_to_insert);
    Cursor* cursor = table_end(table);

    serialize_row(row_to_insert, cursor_value(cursor));
    table->num_rows += 1;

    free(cursor);
    return EXECUTE_SUCCESS;
}

ExecuteResult execute_select(Table *table)
{
    Cursor *cursor = table_start(table);
    Row row;
    while (!(cursor->end_of_table))
    {
        deserialize_row(cursor_value(cursor), &row);
        print_row(&row);
        cursor_advance(cursor);
    }
    free(cursor);
    return EXECUTE_SUCCESS;
}

Table *db_open(const char *filename)
{
    Pager *pager = pager_open(filename);
    uint32_t num_rows = pager->file_length / ROW_SIZE;
    Table *table = malloc(sizeof(Table));
    table->pager = pager;
    table->num_rows = num_rows;
    return table;
}
void db_close(Table *table)
{
    Pager *pager = table->pager;
    uint32_t num_full_pages = table->num_rows / ROWS_PER_PAGE;

    for (uint32_t i = 0; i < num_full_pages; i++)
    {
        if (pager->pages[i] == NULL)
        {
            continue;
        }
        pager_flush(pager, i, PAGE_SIZE);
        free(pager->pages[i]);
        pager->pages[i] = NULL;
    }

    // There may be a partial page to write to the end of the file
    // This should not be needed after we switch to a B-tree
    uint32_t num_additional_rows = table->num_rows % ROWS_PER_PAGE;
    if (num_additional_rows > 0)
    {
        uint32_t page_num = num_full_pages;
        if (pager->pages[page_num] != NULL)
        {
            pager_flush(pager, page_num, num_additional_rows * ROW_SIZE);
            free(pager->pages[page_num]);
            pager->pages[page_num] = NULL;
        }
    }

    int result = close(pager->file_descriptor);
    if (result == -1)
    {
        printf("Error closing db file.\n");
        exit(EXIT_FAILURE);
    }
    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        void *page = pager->pages[i];
        if (page)
        {
            free(page);
            pager->pages[i] = NULL;
        }
    }
    free(pager);
    free(table);
}
Pager *pager_open(const char *filename)
{
    int fd = open(filename, O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);

    if (fd == -1)
    {
        printf("%s", "Enable to open file \n");
        exit(EXIT_FAILURE);
    }
    off_t file_length = lseek(fd, 0, SEEK_END);

    Pager *pager = malloc(sizeof(Pager));
    pager->file_descriptor = fd;
    pager->file_length = file_length;

    for (uint32_t i = 0; i < TABLE_MAX_PAGES; i++)
    {
        pager->pages[i] = NULL;
    }

    return pager;
}
void pager_flush(Pager *pager, uint32_t page_num, uint32_t size)
{
    if (pager->pages[page_num] == NULL)
    {
        printf("Tried to flush null page\n");
        exit(EXIT_FAILURE);
    }

    off_t offset = lseek(pager->file_descriptor, page_num * PAGE_SIZE, SEEK_SET);

    if (offset == -1)
    {
        printf("Error seeking \n");
        exit(EXIT_FAILURE);
    }

    ssize_t bytes_written =
        write(pager->file_descriptor, pager->pages[page_num], size);

    if (bytes_written == -1)
    {
        printf("Error writing \n");
        exit(EXIT_FAILURE);
    }
}

// =============================================== //
Cursor *table_start(Table *table)
{
    Cursor *cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = 0;
    cursor->end_of_table = (table->num_rows == 0);

    return cursor;
}

Cursor *table_end(Table *table)
{
    Cursor *cursor = malloc(sizeof(Cursor));
    cursor->table = table;
    cursor->row_num = table->num_rows;
    cursor->end_of_table = true;

    return cursor;
}

void *cursor_value(Cursor *cursor)
{
    uint32_t row_num = cursor->row_num;
    uint32_t page_num = row_num / ROWS_PER_PAGE;
    void *page = get_page(cursor->table->pager, page_num);
    uint32_t row_offset = row_num % ROWS_PER_PAGE;
    uint32_t byte_offset = row_offset * ROW_SIZE;
    return page + byte_offset;
}

void cursor_advance(Cursor* cursor) {
    cursor->row_num += 1;
    if (cursor->row_num >= cursor->table->num_rows) {
        cursor->end_of_table = true;
    }
}
