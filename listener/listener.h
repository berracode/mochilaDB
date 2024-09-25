#ifndef LISTENER_H
#define LISTENER_H
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <arpa/inet.h>

#include "../utils/mem/mmem.h"
#include "../utils/stdio/logger.h"


int init_server();
void start_server(int server_fd);
int set_fd_nonblocking(int fd);



#endif