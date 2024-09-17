#ifndef HANDLER_H
#define HANDLER_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/select.h>

#include "../data_structures/mhash_table.h"
#include "../config.h"

void handle_connection(int client_fd);
void set_config(config_t *config);
void set_global_hash_table(mhash_table_t *hash_table);

#endif