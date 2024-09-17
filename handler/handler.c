#include "handler.h"
#include "commands.h"
#include "../mem/mmem.h"

extern const char *OK;

mhash_table_t *hash_table;
config_t *config;

void set_config(config_t *config_arg) {
    config = config_arg;
}

void set_global_hash_table(mhash_table_t *hash_table_arg){
    hash_table = hash_table_arg;
}

void handle_connection(int client_fd) {
    printf("Handling client %d\n", client_fd);

    char buffer[config->buffer_size];
    char *output_buffer = NULL;
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
    process_command(buffer, &output_buffer);
    write(client_fd, output_buffer, strlen(output_buffer));
    close(client_fd);

    printf("End Handling client %d\n", client_fd);

}