#ifndef LISTENER_H
#define LISTENER_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/select.h>
#include <arpa/inet.h>

#include "../utils/mem/mmem.h"
#include "../utils/stdio/logger.h"


// borrar
typedef struct server {
    int fd;
    fd_set *read_fds;
    fd_set *master_fds;
} server_t;

int init_server();
void start_server(int server_fd);
void set_fd_nonblocking(int fd);



#endif