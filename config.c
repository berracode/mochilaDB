#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "utils/mem/mmem.h"

#define PORT 7986
#define BUFFER_SIZE 256
#define HASHTABLE_SIZE 512
#define LOAD_FACTOR 0 //0= not rehashing; >0 rehashing

config_t *config = NULL;

char* trim(char* str) {
    char* end;

    while (*str == ' ') str++;

    end = str + strlen(str) - 1;
    while (end > str && *end == ' ') end--;

    *(end + 1) = '\0';

    return str;
}

config_t* init_config() {
    //TODO: implementar función de impresión segura para multithreading
    safe_printf("Reading properties file\n");
    FILE *file = fopen("config.properties", "r");
    if (!file) {
        perror("Error open config file");
        return NULL;
    }

    config_t *config = m_malloc(sizeof(config_t));
    if (!config) {
        perror("Error");
        fclose(file);
        return NULL;
    }
    config->port = PORT;
    config->buffer_size = BUFFER_SIZE;
    config->hashtable_size = HASHTABLE_SIZE;
    config->load_factor = LOAD_FACTOR;

    char line[256];

    while (fgets(line, sizeof(line), file)) {
        // skip comments and empty lins
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }

        char *key = strtok(line, "=");
        char *value = strtok(NULL, "=");

        if (key != NULL && value != NULL) {
            key = trim(key);
            value = trim(value);

            if (strcmp(key, "port") == 0) {
                config->port = atoi(value);
            } else if (strcmp(key, "buffer_size") == 0) {
                config->buffer_size = atoi(value);
            } else if (strcmp(key, "hastable_initial_size") == 0) {
                config->hashtable_size = atoi(value);
            } else if (strcmp(key, "load_factor") == 0) {
                config->load_factor = atof(value);
            } else {
                safe_printf("Unknow key in config file: %s\n", key);
            }
        }
    }
    safe_printf("############################\n");
    safe_printf("### Server configuration ###\n");
    safe_printf("############################\n");

    safe_printf("# Server port: %d\n", config->port);
    safe_printf("# Buffer size: %d\n", config->buffer_size);
    safe_printf("# Initial hashtable size: %d\n", config->hashtable_size);
    safe_printf("# Load factor for rehashing: %.2f\n", config->load_factor);
    safe_printf("############################\n\n");


    fclose(file);
    return config;
}
