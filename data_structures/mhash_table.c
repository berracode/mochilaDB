#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mhash_table.h"
#include "../mem/mmem.h"

void free_entry(entry_t *temp);

mhash_table_t *hash_table;

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

entry_t* htget(mhash_table_t *table, char *key) {
    unsigned int index = hash_function(table->capacity, key);
    entry_t *current = table->entries[index];

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
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

        if (table->entries[index] == NULL) {
            table->entries[index] = new_node;
        } else {
            entry_t *current = table->entries[index];
            entry_t *found = htget(table, key);

            if(found==NULL){ //does not exist
                //added node
                table->entries[index] = new_node;
                new_node->next = current;

            }else{ //exist
                //update value
                m_free(found->value);
                found->value = strdup(value);
                free_entry(new_node);
            }
        }

        table->size++;
    }

}

void free_entry(entry_t *temp){
    if (temp == NULL) {
        return;
    }

    m_free(temp->key);
    m_free(temp->value);
    m_free(temp);

}

void free_table(mhash_table_t *table) {
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
    printf("capacity: %ld - size: %ld\n", table->capacity, table->size);
    for (size_t i = 0; i < table->capacity; i++) {
        entry_t *current = table->entries[i];
        if(current != NULL) {
            while (current != NULL){
                printf("Bucket: %ld [%s, %s]\n", i, current->key, current->value);
                current = current->next;
            }
        } else{
            printf("Bucket %ld VACIO\n", i);
        }
    }
}

