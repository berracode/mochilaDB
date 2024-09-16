#ifndef MEM_H

#include <stddef.h>

void* custom_malloc(size_t size, const char* file, int line);
void custom_free(void* ptr, const char* file, int line);


#define m_malloc(size) custom_malloc(size, __FILE__, __LINE__)
#define m_free(ptr) custom_free(ptr, __FILE__, __LINE__)


#endif