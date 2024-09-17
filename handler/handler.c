#include "handler.h"

mhash_table_t *hash_table;
config_t *config;

void set_config(config_t *config_arg) {
    config = config_arg;
}

void set_global_hash_table(mhash_table_t *hash_table_arg){
    hash_table = hash_table_arg;
}

void print_bucket(entry_t *bucket_head, FILE *file) {
    entry_t *current = bucket_head;
    while (current != NULL) {
        //pthread_rwlock_rdlock(&current->lock);
        fprintf(file, "K=%s, V=%s\n", current->key, current->value);
        current = current->next;
    }
}

void* print_hash_table(void *arg) {
    printf("Hilo DE IMPRESION, ID: %lu\n", pthread_self());  // Imprimir ID del hilo
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

char* read_my(const char *key) {
    printf("--- READING KEY [%s] \n", key);
    char *response;

    entry_t *found = get(hash_table, key);

    if (found!=NULL) {
        return found->value;
    } else {
        response = strdup("No key");
        return response;
    }

}

void handle_connection(int client_fd) {
    printf("Handling client %d\n", client_fd);

    char buffer[config->buffer_size];
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            printf("No data\n");
            return;
        } else {
            perror("read");
            close(client_fd);
            return;
        }
    } else if (bytes_read == 0) {
        close(client_fd);
        return;
    }

    buffer[bytes_read] = '\0';
    printf("--- Handling client 3: %s\n", buffer);

    if (strncmp(buffer, "INSERT", 6) == 0) {
        char *key = strtok(buffer + 7, " ");
        char *value = strtok(NULL, " ");
        printf("--- Handling client 4: %s\n", buffer);

        if (key && value) {
            put(hash_table, key, value);
            write(client_fd, "CIHT", strlen("CIHT"));//Correct Insert in HashTable = CIHT

        }
    } else if (strncmp(buffer, "READ", 4) == 0) {
        char *key = strtok(buffer + 5, " ");
        if (key) {
            char *value = read_my(key);
            printf("#### KEY read: %s\n", value);
            if (value) {
                write(client_fd, value, strlen(value));
            }
        }
    } else if(strncmp(buffer, "LIST", 4)==0) {

        char* filename = "hash_table_output.txt";

        // Crear un nuevo hilo para manejar la operación LIST
        pthread_t list_thread;
        if (pthread_create(&list_thread, NULL, print_hash_table, (void*)filename) != 0) {
            perror("Error al crear el hilo para LIST");
        } else {
            // Hacer detached el hilo para no necesitar hacer join
            pthread_detach(list_thread);
        }
        write(client_fd, "WHW", strlen("WHW"));

    }
    close(client_fd);

    printf("End Handling client %d\n", client_fd);

}