#include "../include/memory.h"
#include "../include/memory_layout.h"
#include <stdint.h>

static uint8_t *free_memory = (uint8_t *)MEMORY_FREE_START;

intptr_t memory_get_kernel_start() { return (intptr_t)&memory_kernel_start; }

intptr_t memory_get_kernel_end() { return (intptr_t)&memory_kernel_end; }

void *memory_alloc(uint64_t size) {
  void *p = free_memory;
  free_memory += size;
  return p;
}
