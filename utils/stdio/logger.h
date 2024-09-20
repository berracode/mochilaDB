#ifndef LOGGER_H
#define LOGGER_H
#include <stdio.h>
#include <stdarg.h>
#include <pthread.h>

void safe_printf(const char* format, ...);


#endif