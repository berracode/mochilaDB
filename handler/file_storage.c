#include "file_storage.h"


void print_bucket(entry_t *bucket_head, FILE *file) {
    entry_t *current = bucket_head;
    while (current != NULL) {
        fprintf(file, "K=%s, V=%s\n", current->key, current->value);
        current = current->next;
    }
}

void* print_hash_table(void *arg) {
    printf("###### Hashtable... %ld\n", hash_table->size);

    char *filename = (char* )arg;
    if (hash_table == NULL) {
        printf("Hash table is NULL\n");
        return NULL;
    }

    // Redirigir stdout al archivo
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    printf("###### Hashtable... %ld\n", hash_table->size);
    for (size_t i = 0; i < hash_table->capacity; ++i) {
        if(hash_table->entries[i]!=NULL){
            fprintf(file, "Bucket %zu:\n", i);
            print_bucket(hash_table->entries[i], file);
        }
    }
    printf("###### HashtableFIN ... %ld\n", hash_table->size);


    fclose(file);
    return NULL;
} //LIST a