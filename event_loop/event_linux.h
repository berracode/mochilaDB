#ifdef __linux__
#include "event_loop.h"
#include <sys/epoll.h>

typedef struct epoll_loop_t {
    event_loop_t base;
    int epoll_fd;
    struct epoll_event event;
    struct epoll_event events[10];
}epoll_loop_t;

void epoll_add(event_loop_t *loop, int fd);

void epoll_remove(event_loop_t *loop, int fd);

void epoll_wait_2(event_loop_t *loop, int fd, handler_connection_t handler);

event_loop_t* create_epoll_loop();
#endif
