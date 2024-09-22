#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>


typedef void (*handler_connection_t)(int client_fd);

typedef struct event_loop_t {
    void (*add)(struct event_loop_t *loop, int fd);
    void (*remove)(struct event_loop_t *loop, int fd);
    void (*wait)(struct event_loop_t *loop, int fd, handler_connection_t handler);
} event_loop_t;


#endif