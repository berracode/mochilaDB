#include "file_storage.h"
#include "../utils/stdio/logger.h"


void print_bucket(entry_t *bucket_head, FILE *file) {
    entry_t *current = bucket_head;
    while (current != NULL) {
        //bloquear
                pthread_rwlock_rdlock(&current->lock);

        fprintf(file, "K=%s, V=%s\n", current->key, current->value);
        //desbloquar
                pthread_rwlock_unlock(&current->lock);

        current = current->next;
    }
}

void* print_hash_table(void *arg) {
    safe_printf("###### Hashtable... %ld\n", hash_table->size);

    char *filename = (char* )arg;
    if (hash_table == NULL) {
        safe_printf("Hash table is NULL\n");
        return NULL;
    }

    // Redirigir stdout al archivo
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return NULL;
    }

    safe_printf("###### Hashtable... %ld\n", hash_table->size);
    for (size_t i = 0; i < hash_table->capacity; ++i) {
        if(hash_table->entries[i]!=NULL){
            fprintf(file, "Bucket %zu:\n", i);
            print_bucket(hash_table->entries[i], file);
        }
    }
    safe_printf("###### HashtableFIN ... %ld\n", hash_table->size);


    fclose(file);
    return NULL;
} //LIST a