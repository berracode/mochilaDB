#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

#define PORT 8080
#define BUFFER_SIZE 256

#define TABLE_SIZE 1024

typedef struct entry {
    char *key;
    char *value;
    pthread_rwlock_t lock;
    struct entry *next;
} entry_t;

typedef struct {
    entry_t **buckets;
    
    pthread_rwlock_t *bucket_locks;  // Array de locks para los buckets
    pthread_rwlock_t table_lock;
} hash_table_t;

unsigned int hash(const char *key) {
    unsigned int hash = 5381;
    int c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash % TABLE_SIZE;
}

hash_table_t *hash_table;


void print_bucket(entry_t *bucket_head, FILE *file) {
    entry_t *current = bucket_head;
    while (current != NULL) {
        //pthread_rwlock_rdlock(&current->lock);
        //printf("  Key: %s, Value: %s\n", current->key, current->value);

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
    for (size_t i = 0; i < TABLE_SIZE; ++i) {
        //pthread_rwlock_rdlock(&hash_table->bucket_locks[i]);
        if(hash_table->buckets[i]!=NULL){
            fprintf(file, "Bucket %zu:\n", i);
            print_bucket(hash_table->buckets[i], file);
        }
        

        //pthread_rwlock_unlock(&hash_table->bucket_locks[i]);
    }

    //pthread_rwlock_unlock(&hash_table->table_lock);

    fclose(file);
} //LIST a

hash_table_t* create_table() {
    printf("Creating hashtable\n");
    hash_table_t *table = malloc(sizeof(hash_table_t));
    if (!table) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    table->buckets = calloc(TABLE_SIZE, sizeof(entry_t *));
    if (!table->buckets) {
        perror("calloc");
        exit(EXIT_FAILURE);
    }

    table->bucket_locks = malloc(TABLE_SIZE * sizeof(pthread_rwlock_t));
    if (!table->bucket_locks) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < TABLE_SIZE; i++) {
        pthread_rwlock_init(&table->bucket_locks[i], NULL);
    }

    pthread_rwlock_init(&table->table_lock, NULL);  // Inicializar el lock de la tabla

    return table;
}

void insert(hash_table_t *table, const char *key, const char *value) {
    unsigned int index = hash(key);
    printf("--- Handling client: key=[%s] - value[%s] - INDEX[%d]\n", key, value, index);


    //pthread_rwlock_wrlock(&table->table_lock);

    entry_t *new_entry = malloc(sizeof(entry_t));
    if (!new_entry) {
        perror("malloc");
        exit(EXIT_FAILURE);
    }
    printf("--- WELL MALLOC\n");


    new_entry->key = strdup(key);
    new_entry->value = strdup(value);
    pthread_rwlock_init(&new_entry->lock, NULL);  // Inicializar el lock de la entrada
    new_entry->next = NULL;

    printf("--- WELL MALLOC 3\n");

    //entry_t *current = table->buckets[index];

    //pthread_rwlock_wrlock(&table->buckets[index]->lock);  // Bloquear el bucket en escritura
    printf("--- WELL MALLOC 4\n");

    
    /*if (current == NULL) {
        table->buckets[index] = new_entry;
    } else {
        while (current->next) {
            current = current->next;
        }
        current->next = new_entry;
    }*/

    if (table->buckets[index] == NULL) {
        //pthread_rwlock_wrlock(&table->bucket_locks[index]);
        printf("Thread %ld acquired 1 read lock\n", pthread_self());

        table->buckets[index] = new_entry;
        //pthread_rwlock_unlock(&table->bucket_locks[index]);  // Desbloquear el bucket
        printf("Thread %ld released 1 read lock\n", pthread_self());
    } else {
        entry_t *current = table->buckets[index];
        //pthread_rwlock_wrlock(&current->lock); // Bloquear la primera entrada 
        printf("Thread %ld acquired 2 read lock\n", pthread_self());

        while (current->next != NULL) {

            if (strcmp(current->key, key) == 0) {
                // La clave ya existe, actualizar el valor
                //pthread_rwlock_wrlock(&current->lock); // Bloquear la entrada actual
                printf("Thread %ld acquired 3 read lock\n", pthread_self());
                free(current->value);
                current->value = strdup(value);
                //pthread_rwlock_unlock(&current->lock); // Desbloquear la entrada actual
                printf("Thread %ld released 3 read lock\n", pthread_self());
                //pthread_rwlock_unlock(&table->bucket_locks[index]); // Desbloquear el bucket
                free(new_entry->key);
                free(new_entry->value);
                free(new_entry);
                return;
            }

            //pthread_rwlock_unlock(&current->lock); //desbloquear la entrada actual
            printf("Thread %ld released 4 read lock\n", pthread_self());

            current = current->next;
            //pthread_rwlock_wrlock(&current->lock);
            printf("Thread %ld acquired 4 read lock\n", pthread_self());

        }
        if(strcmp(current->key, key)==0){ //ya existe la key y es la unica del bucket MEJORAR ESTO
            //pthread_rwlock_wrlock(&current->lock); // Bloquear la entrada actual
            printf("Thread %ld acquired 5 read lock\n", pthread_self());
            free(current->value);
            current->value = strdup(value);
            //pthread_rwlock_unlock(&current->lock); // Desbloquear la entrada actual
            printf("Thread %ld released 5 read lock\n", pthread_self());
            //pthread_rwlock_unlock(&table->bucket_locks[index]); // Desbloquear el bucket
            free(new_entry->key);
            free(new_entry->value);
            free(new_entry);
            return;
        }
        current->next = new_entry;
        //pthread_rwlock_unlock(&current->lock); // Desbloquear el ultimo entry
        printf("Thread %ld released 2 read lock\n", pthread_self());


    }

    printf("--- INSERTED ...\n");

    //pthread_rwlock_unlock(&table->buckets[index]->lock);  // Desbloquear el bucket
    //pthread_rwlock_unlock(&table->table_lock);  // Desbloquear la tabla
}

char* read_my(hash_table_t *table, const char *key) {
    unsigned int index = hash(key);
    printf("--- READING KEY [%s] index[%d]\n", key, index);

    //pthread_rwlock_rdlock(&table->table_lock);  

    //pthread_rwlock_rdlock(&table->buckets[index]->lock);  // Bloquear el bucket en lectura
    if(table->buckets[index] != NULL) {
        entry_t *current = table->buckets[index];
        //pthread_rwlock_rdlock(&current->lock); // Bloquear la primera entrada para escritura
        while (current) {
            //pthread_rwlock_rdlock(&current->lock);
            if (strcmp(current->key, key) == 0) {
                char *value = strdup(current->value);  // Copiar el valor para devolver
                //pthread_rwlock_unlock(&current->lock);  // Desbloquear el entry
                //pthread_rwlock_unlock(&table->table_lock);  // Desbloquear la tabla
                return value;
            }
            //pthread_rwlock_unlock(&current->lock);  // Desbloquear el entry
            current = current->next;

        }

    } else {
        char *response= strdup("No key\n");
        return response;
    }

    
    //pthread_rwlock_unlock(&table->buckets[index]->lock);  // Desbloquear el bucket
    //pthread_rwlock_unlock(&table->table_lock);  // Desbloquear la tabla
    return NULL;  // Clave no encontrada
}

void destroy_table() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        entry_t *current = hash_table->buckets[i];
        while (current) {
            entry_t *next = current->next;
            pthread_rwlock_destroy(&current->lock);
            free(current->key);
            free(current->value);
            free(current);
            current = next;
        }
    }

    pthread_rwlock_destroy(&hash_table->table_lock);
    free(hash_table->buckets);
    free(hash_table);
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
    printf("Handling client2 %d\n", client_fd);

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
            insert(hash_table, key, value);
            write(client_fd, "CIHT", strlen("CIHT"));//Correct Insert in HashTable = CIHT

        }
    } else if (strncmp(buffer, "READ", 4) == 0) {
        char *key = strtok(buffer + 5, " ");
        if (key) {
            char *value = read_my(hash_table, key);
            printf("#### KEY read: %s\n", value);
            if (value) {
                write(client_fd, value, strlen(value));
                free(value);
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
    hash_table = create_table();

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

    destroy_table();
    close(server_fd);
    return 0;
}