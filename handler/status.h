#ifndef STATUS_H
#define STATUS_H

extern const char *OK;
extern const char *ERROR_SETTING;
extern const char *UNKNOWN_COMMAND;
extern const char *NOT_FOUND;

typedef enum {
    SUCCESS,
    ERROR_KEY_NOT_FOUND,
    ERROR_DATABASE_FULL,
    ERROR_SETTING_ENTRY,
    ERROR_INVALID_ARGUMENTS
} operation_status_t;

typedef struct {
    operation_status_t status;
    char *message; // Mensaje descriptivo del estado
} response_t;

#endif