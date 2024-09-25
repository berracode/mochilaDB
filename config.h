#ifndef CONFIG_H
#define CONFIG_H

#include "utils/stdio/logger.h"

//to convert in hash table too
typedef struct config {
    int port;
    int buffer_size;
    int hashtable_size;
    float load_factor;
} config_t;
extern config_t *config;

config_t* init_config();

#endif