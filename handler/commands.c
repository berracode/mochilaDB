#include "commands.h"
#include "../mem/mmem.h"
#include "../data_structures/mhash_table.h"
#include "file_storage.h"

#define MAX_ARGS 10

command_t commands[] = {
    {"set", set},
    {"get", get},
    {"list", list}
};
const int NUM_COMMANDS = sizeof(commands) / sizeof(command_t);


response_t* set(int argc, char *argv[]) {
    response_t *response = m_malloc(sizeof(response_t *));
    printf("Executing command %s with %d arguments\n",argv[0], argc);
    char *key = argv[1];
    char *value = argv[2];
    if (key && value) {
        put(hash_table, key, value);
    }

    response->status = SUCCESS;
    response->message = strdup(OK);

    return response;

}

response_t* get(int argc, char *argv[]) {
    printf("Executing command %s with %d arguments\n",argv[0], argc);
    response_t *response = m_malloc(sizeof(response_t *));

    char *key = argv[1];
    if (key) {
        entry_t *entry = htget(hash_table, key);
        if (!entry) {
            response->status = ERROR_KEY_NOT_FOUND;
            response->message = strdup(NOT_FOUND);
        }else{
            response->message = strdup(entry->value);
            response->status = SUCCESS;
        }
    }

    return response;
}

response_t* list(int argc, char *argv[]) {
    printf("Executing command %s with %d arguments\n",argv[0], argc);
    response_t *response = m_malloc(sizeof(response_t *));

    char* filename = "hash_table_output.txt";
    pthread_t list_thread;
    if (pthread_create(&list_thread, NULL, print_hash_table, (void*)filename) != 0) {
        perror("Error");
    } else {
        pthread_detach(list_thread);
        response->status = SUCCESS;
        response->message = strdup(OK);
    }
    return response;
}

void clean_argv(int argc, char *argv[]) {
    int last_token_len = strlen(argv[argc-1]);
    
    if(argv[argc-1][last_token_len-1]==10
        && argv[argc-1][last_token_len-2]==13){
        argv[argc-1][last_token_len-2]= '\0';
    }

    /*for (int k = 0; k < (int)strlen(argv[argc-1]); k++)
    {
        printf("last token: [%d]\n", argv[argc-1][k]);


    }
    

    for (int i = 0; i < argc; i++){
        printf("---- %s\n", argv[i]);
        for (int j = 0; j < (int)strlen(argv[i]); j++){
            printf("[%d]\n", argv[i][j]);
        }
        printf("----\n");
    }*/

}

void process_command(char *input, char **output) {
    char *argv[MAX_ARGS];
    int argc = 0;

    char *token = strtok(input, " ");
    while (token != NULL && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    if (argc == 0) {
        printf("None command\n");
        return;
    }
    //TODO: improve commands proccessing
    //TODO: allow multiwords value, example set key "value with multiple words"
    clean_argv(argc, argv);

    for (int i = 0; i < NUM_COMMANDS; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            // Ejecutar el comando encontrado
            response_t *response = commands[i].execute(argc, argv);
            *output = strdup(response->message);
            m_free(response->message);
            m_free(response);
            return;
        }
    }

    *output = strdup(UNKNOWN_COMMAND);
    printf("%s\n", *output);

}
