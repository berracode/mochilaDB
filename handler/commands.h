
#ifndef COMMANDS_H
#define COMMANDS_H

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>

#include "status.h"

typedef response_t* (*command_function)(int argc, char *argv[]);

typedef struct {
    const char *name;
    command_function execute;
} command_t;



void process_command(char *input, char **output);
response_t* set(int argc, char *argv[]);

response_t* get(int argc, char *argv[]);

response_t* list(int argc, char *argv[]);


#endif