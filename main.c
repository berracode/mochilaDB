#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "data_structures/mhash_table.h"
#include "mem/mmem.h"

#define PORT 8080
#define BUFFER_SIZE 256

mhash_table_t *hash_table;


void print_bucket(entry_t *bucket_head, FILE *file) {
    entry_t *current = bucket_head;
    while (current != NULL) {
        //pthread_rwlock_rdlock(&current->lock);
        printf("  Key: %s, Value: %s\n", current->key, current->value);

        fprintf(file, "K=%s, V=%s\n", current->key, current->value);
        //fprintf(file, "K: %s, V: %s\n", current->key, current->value);

        //pthread_rwlock_unlock(&current->lock);

        current = current->next;
    }
}

void print_hash_table(const char *filename) {
    if (hash_table == NULL) {
        printf("Hash table is NULL\n");
        return;
    }

    // Redirigir stdout al archivo
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Failed to open file");
        return;
    }

    //pthread_rwlock_rdlock(&hash_table->table_lock);
    //fprintf(file, "Hash Table:\n");
    printf("Hashtable... %ld\n", hash_table->size);
    for (size_t i = 0; i < hash_table->capacity; ++i) {
        if(hash_table->entries[i]!=NULL){
            fprintf(file, "Bucket %zu:\n", i);
            print_bucket(hash_table->entries[i], file);
        }
        

        //pthread_rwlock_unlock(&hash_table->bucket_locks[i]);
    }

    //pthread_rwlock_unlock(&hash_table->table_lock);

    fclose(file);
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


void set_fd_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int init_server() {
    printf("Starting server...\n");

    int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt SO_REUSEADDR");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt SO_REUSEPORT");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server started successful!\n");
    return server_fd;
}

void handle_connection(int client_fd) {
    printf("Handling client %d\n", client_fd);

    char buffer[BUFFER_SIZE];
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
                m_free(value);
            }
        }
    } else if(strncmp(buffer, "LIST", 4)==0) {
        print_hash_table("hash_table_output.txt");
        write(client_fd, "WHW", strlen("WHW"));

    }
    close(client_fd);

    printf("End Handling client %d\n", client_fd);

}

int main() {
    hash_table = create_table(512);

    int server_fd = init_server();
    set_fd_nonblocking(server_fd);

    int max_fd = server_fd;
    fd_set read_fds, master_fds;

    FD_ZERO(&master_fds);
    FD_SET(server_fd, &master_fds);

    struct timeval timeout;
    timeout.tv_sec = 5;  // 5 segundos
    timeout.tv_usec = 0; // 0 microsegundos


    while (1) {
        read_fds = master_fds;

        if (select(max_fd + 1, &read_fds, NULL, NULL, &timeout) < 0) {
            perror("select");
            exit(EXIT_FAILURE);
        }

       for (int fd = 0; fd <= max_fd; fd++) {
            if (FD_ISSET(fd, &read_fds)) {
                if (fd == server_fd) {
                    struct sockaddr_in address;
                    socklen_t addrlen = sizeof(address);
                    int new_socket = accept(server_fd, (struct sockaddr *)&address, &addrlen);

                    if (new_socket < 0) {
                        perror("accept");
                        continue;
                    }

                    set_fd_nonblocking(new_socket);
                    FD_SET(new_socket, &master_fds);
                    if (new_socket > max_fd) {
                        max_fd = new_socket;
                    }
                } else {
                    handle_connection(fd);
                    FD_CLR(fd, &master_fds);
                }
            }
        }
    }

    free_table(hash_table);
    close(server_fd);
    return 0;
}