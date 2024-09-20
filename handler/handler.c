#include "handler.h"
#include "commands.h"
#include "../utils/mem/mmem.h"
#include "../utils/stdio/logger.h"

void handle_connection(int client_fd) {
    safe_printf("Handling client %d\n", client_fd);

    char buffer[config->buffer_size];
    char *output_buffer = NULL;
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

    if (bytes_read < 0) {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            safe_printf("No data\n");
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

    safe_printf("End Handling client %d\n", client_fd);

}