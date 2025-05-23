#ifndef DB_H
#define DB_H

typedef struct Entry
{
    char *key;
    char *value;
    struct Entry *next ;

}Entry;

typedef struct 
{
    /* data */
    Entry **buckets;
    int size;

}HashTable;

HashTable *create_table(int size);
void free_table(HashTable *table);

void put(HashTable *table, const char *key, const char *value);
char *get(HashTable *table, const char *key);
void deletee(HashTable *table, const char *key);

#endif