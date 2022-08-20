#ifndef _MEMORY_H
#define _MEMORY_H
#include "stdint.h"

extern char memory_kernel_start;
extern char memory_kernel_end;

intptr_t memory_get_kernel_start();
intptr_t memory_get_kernel_end();
void *memory_alloc(uint64_t size);

#endif
