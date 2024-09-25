#ifdef __APPLE__
#include "event_macos.h"

void kqueue_add(event_loop_t *loop, int fd) {
    kqueue_loop_t *kqueue_loop = (kqueue_loop_t *)loop;
    EV_SET(&kqueue_loop->ev_set, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
    kevent(kqueue_loop->kq_fd, &kqueue_loop->ev_set, 1, NULL, 0, NULL);
}

void kqueue_remove(event_loop_t *loop, int fd) {
    kqueue_loop_t *kqueue_loop = (kqueue_loop_t *)loop;
    EV_SET(&kqueue_loop->ev_set, fd, EVFILT_READ, EV_DELETE, 0, 0, NULL);
    kevent(kqueue_loop->kq_fd, &kqueue_loop->ev_set, 1, NULL, 0, NULL);
}

void kqueue_wait_2(event_loop_t *loop, int server_fd, handler_connection_t handler) {
    kqueue_loop_t *kqueue_loop = (kqueue_loop_t *)loop;
    int n = kevent(kqueue_loop->kq_fd, NULL, 0, kqueue_loop->ev_list, 10, NULL);
    if (n < 0) {
        perror("Error en kevent");
        return;
    }

    for (int i = 0; i < n; i++) {
        if (kqueue_loop->ev_list[i].ident == (uintptr_t)server_fd) {
            int client_fd = accept(server_fd, NULL, NULL);
            if (client_fd < 0) {
                perror("Error accepting client");
                continue;
            }
            kqueue_add(loop, client_fd);

        } else {
            handler(kqueue_loop->ev_list[i].ident);
        }
    }
}

struct event_loop_t *create_kqueue_loop() {
    kqueue_loop_t *kqueue_loop = malloc(sizeof(kqueue_loop_t));
    kqueue_loop->kq_fd = kqueue();
    kqueue_loop->base.add = kqueue_add;
    kqueue_loop->base.remove = kqueue_remove;
    kqueue_loop->base.wait = kqueue_wait_2;
    return (event_loop_t *)kqueue_loop;
}
#endif
