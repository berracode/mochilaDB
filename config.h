#ifndef CONFIG_H
#define CONFIG_H

//to convert in hash table too
typedef struct config {
    int port;
    int buffer_size;
    int hashtable_size;
    float load_factor;
    int thread_pool_size;
} config_t;
extern config_t *config;

config_t* init_config();

#endif