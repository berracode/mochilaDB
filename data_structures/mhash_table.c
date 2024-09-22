#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mhash_table.h"
#include "../utils/stdio/logger.h"

void free_entry(entry_t *temp);

mhash_table_t *hash_table = NULL;

mhash_table_t* create_table(size_t initial_capacity) {
    mhash_table_t *table = (mhash_table_t *)m_malloc(sizeof(mhash_table_t));
    if (!table) return NULL;
    table->entries = (entry_t **)calloc(initial_capacity , sizeof(entry_t *));
    if (!table->entries) {
        m_free(table);
        return NULL;
    }
    table->size = 0;
    table->capacity = initial_capacity;
    pthread_rwlock_init(&table->table_lock, NULL);
    return table;
}

unsigned int hash_function(const size_t hash_table_size, const char *str) {
    unsigned int hash = 5381;
    int c;

    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;

    unsigned int n_hash = hash % hash_table_size;

    return n_hash;
}

char* htget(mhash_table_t *table, char *key) {
    char *value = NULL;
    unsigned int index = hash_function(table->capacity, key);
    entry_t *current = table->entries[index];

    if(!current) return NULL;
    while (current != NULL) {
        pthread_rwlock_rdlock(&current->lock);

        if (strcmp(current->key, key) == 0) {
            value = strdup(current->value);
            pthread_rwlock_unlock(&current->lock);
            return value;
        }
        current = current->next;
        pthread_rwlock_unlock(&current->lock);


    }

    return NULL; // Key not found
}

void put(mhash_table_t *table, char *key, char *value) {
    if(key != NULL && value != NULL){
        unsigned int index= hash_function(table->capacity, key);

        entry_t *new_node = (entry_t *)m_malloc(sizeof(entry_t));
        new_node->key = (char*)m_malloc(strlen(key) + 1);
        strcpy(new_node->key, key);

        new_node->value = (char *)m_malloc(strlen(value) + 1);
        strcpy(new_node->value, value);

        new_node->next = NULL;
        pthread_rwlock_init(&new_node->lock, NULL);

        if (table->entries[index] == NULL) { //TODO: bloquear bucket no tabla
            pthread_rwlock_wrlock(&table->table_lock);
            table->entries[index] = new_node;
            pthread_rwlock_unlock(&table->table_lock);
        } else { // si hay datos en el bcuket
            entry_t *current = table->entries[index];
            while (current != NULL) {
                pthread_rwlock_wrlock(&current->lock);  // Lock de escritura para la entrada
                if (strcmp(current->key, key) == 0) {
                    m_free(current->value);
                    current->value = (char *)m_malloc(strlen(value) + 1);
                    strcpy(current->value, value);  // Actualiza con el nuevo valor
                    pthread_rwlock_unlock(&current->lock);
                    free_entry(new_node);
                    break;
                }
                pthread_rwlock_unlock(&current->lock);
                current = current->next;
            }

            if (current == NULL) {
                // Si no se encontró la clave, agregar una nueva entrada
                pthread_rwlock_wrlock(&table->entries[index]->lock);  // Lock de escritura para la entrada
                new_node->next = table->entries[index];
                table->entries[index] = new_node;
                table->size++;
                pthread_rwlock_unlock(&table->entries[index]->lock);
            }
        }

    }

}

void free_entry(entry_t *temp){
    if (temp == NULL) {
        return;
    }
    pthread_rwlock_destroy(&temp->lock);
    m_free(temp->key);
    m_free(temp->value);
    m_free(temp);

}

void free_table(mhash_table_t *table) {
    pthread_rwlock_destroy(&table->table_lock);
    for (size_t i = 0; i < table->capacity; i++) {
        entry_t *current = table->entries[i];
        while (current != NULL) {
            entry_t *temp = current;
            current = current->next;
            free_entry(temp);
        }
    }
    m_free(table->entries);
    m_free(table);
}



void print_table(const mhash_table_t *table) {
    safe_printf("capacity: %ld - size: %ld\n", table->capacity, table->size);
    for (size_t i = 0; i < table->capacity; i++) {
        entry_t *current = table->entries[i];
        if(current != NULL) {
            while (current != NULL){
                safe_printf("Bucket: %ld [%s, %s]\n", i, current->key, current->value);
                current = current->next;
            }
        } else{
            safe_printf("Bucket %ld VACIO\n", i);
        }
    }
}

