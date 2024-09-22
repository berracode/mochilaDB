#include <signal.h>
#include <stdbool.h>
#include "listener.h"
#include "../event_loop/event_loop.h"
#include "../event_loop/event_linux.h"
#include "../data_structures/mhash_table.h"
#include "../handler/handler.h"

int server_fd;
volatile bool keep_running = true;
event_loop_t *loop;

event_loop_t* create_event_loop() {
#ifdef __linux__
    return create_epoll_loop();
#elif defined(__APPLE__)
    return create_kqueue_loop();
#else
    #error "Unsupported platform"
#endif
}


void set_fd_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int init_server() {
    safe_printf("Starting server\n");
    config = init_config();

    server_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
    if (server_fd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    /*int opt = 1;
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt SO_REUSEADDR");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt SO_REUSEPORT");
        close(server_fd);
        exit(EXIT_FAILURE);
    }*/

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config->port);

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
    safe_printf("Server started successful!\n");
    return server_fd;
}

void cleanup() {
    safe_printf(" 1 Resources cleaned up and server stopped.\n");

    if (hash_table) {
        free_table(hash_table);
    }
    safe_printf(" 2 Resources cleaned up and server stopped.\n");

    if (config) {
        m_free(config);
    }

    safe_printf("Resources cleaned up and server stopped.\n");
}

// Manejador de la señal SIGINT
void signal_handler(int sig) {
    shutdown(server_fd, SHUT_RDWR);
    keep_running = false;
    if (sig == SIGINT) {
        safe_printf("SIGINT received. Cleaning up resources...\n");
    } else if (sig == SIGABRT) {
        safe_printf("SIGABRT received. Cleaning up resources...\n");
    } else if (sig == SIGTERM) {
        safe_printf("SIGTERM received. Cleaning up resources...\n");
    } else {
        safe_printf("Signal %d received. Cleaning up resources...\n", sig);
    }
    cleanup();
    // Liberar memoria (si es necesario)
    free(loop);
     if (server_fd >= 0) {
        close(server_fd);
    }

    close(server_fd);
    exit(0);  // Salir del programa
}

void start_server(int server_fd){
    // Registrar el manejador de SIGINT
    signal(SIGINT, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGTERM, signal_handler);

    hash_table = create_table(config->hashtable_size);

    loop = create_event_loop();

    //set_config(config);
    set_fd_nonblocking(server_fd);

    loop->add(loop, server_fd);

    /*int max_fd = server_fd;
    fd_set read_fds, master_fds;

    FD_ZERO(&master_fds);
    FD_SET(server_fd, &master_fds);

    struct timeval timeout;
    timeout.tv_sec = 0;  // 5 segundos
    timeout.tv_usec = 0; // 0 microsegundos*/


    while (keep_running) {
        // Esperar eventos
        loop->wait(loop, server_fd, handle_connection);

    }
    printf("LLega aqui\n");
    if (server_fd >= 0) {
        close(server_fd);
    }

    close(server_fd);
    printf("LLega aqui 2\n");


}