#include <stdlib.h>
#include <stdio.h>

#include "mmem.h"

void* custom_malloc(size_t size, const char* file, int line) {
    void* ptr = malloc(size);
    if (!ptr) {
        fprintf(stderr, "Error: Can't set memory in %s in line %d\n", file, line);
        exit(EXIT_FAILURE);
    }
    //printf("\nSet memory: %zu bytes in %s line %d\n\n", size, file, line);
    return ptr;
}

void custom_free(void* ptr, const char* file, int line) {
    if (ptr) {
        free(ptr);
        //printf("Freed memory in %s line %d\n", file, line);
    } else {
        fprintf(stderr, "Warning: Attempted to free a NULL pointer in %s line %d\n", file, line);
    }
}
