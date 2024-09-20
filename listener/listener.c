#include "listener.h"
#include "../data_structures/mhash_table.h"
#include "../data_structures/mqueue.h"
#include "../handler/handler.h"

void set_fd_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

int init_server() {
    safe_printf("Starting server\n");
    config = init_config();

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
    safe_printf("Hilo esperando %ld\n", pthread_self());
    while (1) {
        int client_fd = dequeue(request_queue);
        handle_connection(client_fd);
    }
    return NULL;
}

void start_server(int server_fd){

    hash_table = create_table(config->hashtable_size);
    //queue
    request_queue = init_queue();

    pthread_t pool[config->thread_pool_size];
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


    while (1) {
        read_fds = master_fds;
        //TODO: implementar el eventloop con epoll y kqueue
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
                    //handle_connection(fd); 
                    enqueue(request_queue, fd);
                    FD_CLR(fd, &master_fds);
                }
            }
        }
    }
    destroy_queue(request_queue);
    close(server_fd);
    free_table(hash_table);
    m_free(config);

}