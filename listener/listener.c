#include <signal.h>
#include <stdbool.h>
#include "listener.h"
#include "../event_loop/event_loop.h"
#include "../event_loop/event_linux.h"
#include "../event_loop/event_macos.h"
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

int set_fd_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("Error en nonblcoking");
        return -1;
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("Error al configurar el socket no bloqueante");
        return -1;
    }

    return 0;
}


int init_server() {
    safe_printf("Starting server\n");
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if (set_fd_nonblocking(server_fd) < 0) {
        close(server_fd);
        exit(EXIT_FAILURE);
    }
#ifdef __APPLE__
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
#endif
    config = init_config();

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(config->port);

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 1024) < 0) {
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

    m_free(loop);
    if (server_fd >= 0) {
        close(server_fd);
    }

    close(server_fd);
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
    exit(0);  // Salir del programa
}

void start_server(int server_fd){
    // Registrar el manejador de SIGINT
    signal(SIGINT, signal_handler);
    signal(SIGABRT, signal_handler);
    signal(SIGTERM, signal_handler);

    hash_table = create_table(config->hashtable_size);

    loop = create_event_loop();
    loop->add(loop, server_fd);

//TODO: combinar con multithread y comprar rendimiento
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