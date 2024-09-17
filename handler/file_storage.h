#ifndef FILE_STORAGE_H
#define FILE_STORAGE_H
#include <stdio.h>
#include <pthread.h>
#include "../data_structures/mhash_table.h"

void print_bucket(entry_t *bucket_head, FILE *file);
void* print_hash_table(void *arg);


#endif