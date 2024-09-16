#ifndef MHASH_TABLE_H
#define MHASH_TABLE_H
#include <stdlib.h>

typedef struct entry {
    char *key;
    char *value;
    struct entry *next;
} entry_t;

typedef struct kthash_table_t{
    entry_t **entries;
    size_t size;
    size_t capacity;
} mhash_table_t;

mhash_table_t* create_table(size_t initial_capacity);

void put(mhash_table_t *table, char *key, char *value);

void free_table(mhash_table_t *table);

void print_table(const mhash_table_t *table);

entry_t* get(mhash_table_t *table, const char *key);


#endif