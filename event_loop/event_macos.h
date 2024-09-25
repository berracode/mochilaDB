#ifdef __APPLE__
#include "event_loop.h"
#include <sys/event.h>

// TODO: investigar como sería la implementación en windows
typedef struct kqueue_loop_t {
    event_loop_t base;
    int kq_fd;
    struct kevent ev_set;
    struct kevent ev_list[10]; //TODO: puedo buscar la forma de parametrizar esto
} kqueue_loop_t;

void kqueue_add(event_loop_t *loop, int fd);

void kqueue_remove(event_loop_t *loop, int fd);

void kqueue_wait_2(event_loop_t *loop, int server_fd, handler_connection_t handler);

struct event_loop_t *create_kqueue_loop();
#endif