#ifndef HANDLER_H
#define HANDLER_H
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <sys/socket.h>


#include "../data_structures/mhash_table.h"
#include "../config.h"

void handle_connection(int client_fd);
void set_config(config_t *config);

#endif