#ifndef MHASH_TABLE_H
#define MHASH_TABLE_H
#include <stdlib.h>
#include "../utils/mem/mmem.h"


typedef struct entry {
    char *key;
    char *value;
    struct entry *next;
} entry_t;

typedef struct kthash_table_t{
    entry_t **entries;
    size_t size;
    size_t capacity; //TODO: implementar rehashing
} mhash_table_t; //TODO: hacer que está estructura y sus metodos sea segura concurrentemente, usar RWLOCK CON BLOQUEO FINO

extern mhash_table_t *hash_table;

mhash_table_t* create_table(size_t initial_capacity);

int put(mhash_table_t *table, char *key, char *value);

void free_table(mhash_table_t *table);

void print_table(const mhash_table_t *table);

char* htget(mhash_table_t *table, char *key);


#endif