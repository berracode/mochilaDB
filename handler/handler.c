#include "handler.h"
#include "commands.h"
#include "../utils/mem/mmem.h"
#include "../utils/stdio/logger.h"

void handle_connection(int client_fd) {
    safe_printf("Handling client %d\n", client_fd);

    char buffer[config->buffer_size];
    char *output_buffer = NULL;
    ssize_t bytes_read = read(client_fd, buffer, sizeof(buffer) - 1);

    /*if (bytes_read < 0) {
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
    }*/

    if(bytes_read > 0){
        buffer[bytes_read] = '\0';
        process_command(buffer, &output_buffer);
        //ssize_t bytes_written = write(client_fd, output_buffer, strlen(output_buffer));
        ssize_t bytes_sent = send(client_fd, output_buffer, strlen(output_buffer), MSG_NOSIGNAL);
        if (bytes_sent < 0) {
            perror("send");
            // Manejar el error
        }

        safe_printf("End Handling client %d, %ld\n", client_fd, bytes_sent);
    }
   

    shutdown(client_fd, SHUT_RDWR);
    close(client_fd);
    m_free(output_buffer);

}