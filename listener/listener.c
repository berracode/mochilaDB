#include <signal.h>
#include "listener.h"
#include "../data_structures/mhash_table.h"
#include "../data_structures/mqueue.h"
#include "../handler/handler.h"

pthread_t *pool;
volatile bool keep_running = true;
int server_fd;

void set_fd_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int init_server() {
    safe_printf("Starting server\n");
    config = init_config();

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
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

void* worker_thread(void* arg) {
    while (keep_running) {
        int client_fd = dequeue(request_queue);
        if(client_fd){
            handle_connection(client_fd);
        }
    }
    safe_printf("Thread %ld is stopping..\n", pthread_self());
    return NULL;
}

void cleanup() {
    if (request_queue) {
        destroy_queue(request_queue);
    }
        safe_printf(" 1 Resources cleaned up and server stopped.\n");

    if (hash_table) {
        free_table(hash_table);
    }
    safe_printf(" 2 Resources cleaned up and server stopped.\n");

    for (int i = 0; i < config->thread_pool_size; i++) {
        safe_printf("-----------$$$$$$----- AQUI th\n");
        pthread_join(pool[i], NULL);
    }
    if (config) {
        m_free(config);
    }
    m_free(pool);
    if(request_queue){
        m_free(request_queue);
    }
    safe_printf("Resources cleaned up and server stopped.\n");
}

// Manejador de la señal SIGINT
void signal_handler(int sig) {
    shutdown(server_fd, SHUT_RDWR);

    keep_running = false;
    safe_printf("KEEP_RUNNING %d\n", keep_running);
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
    //queue
    request_queue = init_queue();

    pool = (pthread_t *)malloc(config->thread_pool_size * sizeof(pthread_t));
    for (int i = 0; i < config->thread_pool_size; i++) {
        pthread_create(&pool[i], NULL, worker_thread, NULL);
    }

    //set_config(config);
    set_fd_nonblocking(server_fd);

    int max_fd = server_fd;
    fd_set read_fds, master_fds;

    FD_ZERO(&master_fds);
    FD_SET(server_fd, &master_fds);

    struct timeval timeout;
    timeout.tv_sec = 5;  // 5 segundos
    timeout.tv_usec = 0; // 0 microsegundos


    while (keep_running) {
        //safe_printf("-------------------------- CORRIENDO EN BUCLE PRINCIAPL\n");
        read_fds = master_fds;
        //TODO: implementar el eventloop con epoll y kqueue
        int activity = select(max_fd + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0 && errno != EINTR) {  // Ignorar interrupciones por señales
            perror("select");
            break;
        }

        if (!keep_running) {
            safe_printf("---------------- TOCA SALIR\n");
            break;  // Salir del bucle si se ha recibido SIGINT o SIGTERM
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
                    //handle_connection(fd); 
                    enqueue(request_queue, fd);
                    FD_CLR(fd, &master_fds);
                }
            }
        }
    }

    if (server_fd >= 0) {
        close(server_fd);
    }

    destroy_queue(request_queue);
    close(server_fd);
    free_table(hash_table);
    m_free(config);

}