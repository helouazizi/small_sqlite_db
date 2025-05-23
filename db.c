#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "db.h"

// this is a basic hash algorithm
unsigned long hash(const char *str)
{
    unsigned long hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash;
}

char *my_strdup(const char *s) {
    if (s == NULL) return NULL;
    size_t len = strlen(s) + 1;
    char *dup = malloc(len);
    if (dup != NULL) {
        memcpy(dup, s, len);
    }
    return dup;
}
// lets create our table
HashTable *create_table(int size)
{
    // malloc just alocate memory
    HashTable *table = malloc(sizeof(HashTable));
    table->size = size;
    // the calloc allocate memory and zeroed it
    // to avoid nil pointer memory
    table->buckets = calloc(size, sizeof(Entry *));
    return table;
}

// free up or table
void free_table(HashTable *table)
{
    for (size_t i = 0; i <( size_t) table->size; i++)
    {
        Entry *entry = table->buckets[i];
        while (entry)
        {
            Entry *tem = entry;
            entry = entry->next;
            free(tem->key);
            free(tem->value);
            free(tem);
        }
    }
    free(table->buckets);
    free(table);
}

// lets fill our table

void put(HashTable *table, const char *key, const char *value)
{
    unsigned long index = hash(key) % table->size;
    Entry *entry = table->buckets[index];
    while (entry)
    {
        // if it exist and matched the keys overithee the prviouse value
        if (strcmp(entry->key, key))
        {
            free(entry->key);
            entry->value = my_strdup(value);
            return;
        }

        // let the while finnd the key to match
        entry = entry->next;
    }

    // in this vase out off while the key does not exist in the table
    // lets create it
    Entry *new_entry = malloc(sizeof(Entry));
    new_entry->key = my_strdup(key);
    new_entry->value = my_strdup(value);
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
}

// for geting the values from the table
char *get(HashTable *table, const char *key)
{
    unsigned long index = hash(key) % table->size;
    Entry *entry = table->buckets[index];

    while (entry)
    {
        if (strcmp(entry->key, key) == 0)
        {
            return entry->value;
        }
        entry = entry->next;
    }
    return NULL;
}

void deletee(HashTable *table, const char *key)
{
    unsigned long index = hash(key) % table->size;
    Entry *entry = table->buckets[index];
    Entry *prev = NULL;

    while (entry)
    {
        if (strcmp(entry->key, key) == 0)
        {
            if (prev)
                prev->next = entry->next;
            else
                table->buckets[index] = entry->next;

            free(entry->value);
            free(entry->key);
            free(entry);
            return;
        }
        prev = entry;
        entry = entry->next;
    }
}



