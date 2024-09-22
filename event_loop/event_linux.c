#ifdef __linux__
#include "event_linux.h"

void epoll_add(event_loop_t *loop, int fd) {
    epoll_loop_t *epoll_loop = (epoll_loop_t *)loop;
    epoll_loop->event.events = EPOLLIN;
    epoll_loop->event.data.fd = fd;
    epoll_ctl(epoll_loop->epoll_fd, EPOLL_CTL_ADD, fd, &epoll_loop->event);
}

void epoll_remove(event_loop_t *loop, int fd) {
    epoll_loop_t *epoll_loop = (epoll_loop_t *)loop;
    epoll_ctl(epoll_loop->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
}

void epoll_wait_2(event_loop_t *loop, int server_fd, handler_connection_t handler) {
    epoll_loop_t *epoll_loop = (epoll_loop_t *)loop;
    int n = epoll_wait(epoll_loop->epoll_fd, epoll_loop->events, 10, -1);
    if (n < 0) {
        perror("Error en epoll_wait");
        return;
    }

    for (int i = 0; i < n; i++) {
        for (int i = 0; i < n; i++) {
            if (epoll_loop->events[i].data.fd == server_fd) {
                // Nueva conexión
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd < 0) {
                    perror("Error en accept");
                    continue;
                }

                printf("Nueva conexión de cliente: %d\n", client_fd);

                // Agregar el descriptor del cliente al epoll
                epoll_add(loop, client_fd);

            } else {
                // AQUI PONER FUNCION HANDLE CONNECTION
               handler(epoll_loop->events[i].data.fd);
               //loop->remove(loop, epoll_loop->events[i].data.fd); //TODO: VALIDAR SI DEBO CERRAR SOCKET Y REMOVER

            }
        }
    }
}

event_loop_t *create_epoll_loop() {
    epoll_loop_t *epoll_loop = malloc(sizeof(epoll_loop_t));
    epoll_loop->epoll_fd = epoll_create1(0);
    epoll_loop->base.add = epoll_add;
    epoll_loop->base.remove = epoll_remove;
    epoll_loop->base.wait = epoll_wait_2;
    return (event_loop_t *)epoll_loop;
}
#endif
