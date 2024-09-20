#include "logger.h"

pthread_mutex_t print_mutex = PTHREAD_MUTEX_INITIALIZER;


void safe_printf(const char* format, ...) {
    pthread_mutex_lock(&print_mutex);
    va_list args;
    va_start(args, format);
    vprintf(format, args);
    va_end(args);
    pthread_mutex_unlock(&print_mutex);
}