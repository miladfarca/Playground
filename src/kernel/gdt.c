#include "../include/gdt.h"
#include "../include/memory_layout.h"
#include <stdint.h>

static uint16_t gdt_size = 0x18; // in bytes.
static uint8_t *gdtr = (uint8_t *)MEMORY_GDTR_START;
static uint64_t *gdt_content = (uint64_t *)MEMORY_GDT_START;

void gdt_set() {
  // Memory layout:
  //
  // MEMORY_GDTR_START | 6 bytes (4 bytes address + 2 bytes limit (size -1))
  //                   |
  // MEMORY_GDT_START  | 8 bytes gdt entry 0
  //                   |
  //                   | 8 bytes gdt entry 1
  //                   | ...
  //
  // size is hard coded to be 3 entries.
  //
  // https://wiki.osdev.org/GDT
  // https://wiki.osdev.org/GDT_Tutorial
  // https://stackoverflow.com/a/23979175

  *gdt_content = 0x0;
  // kernel code segment.
  *(gdt_content + 1) = 0xCF9E000000FFFF;
  // kernel data segment.
  *(gdt_content + 2) = 0xCF92000000FFFF;

  // setup gdt content.
  *(uint16_t *)gdtr = gdt_size - 1;
  *(uint32_t *)(gdtr + 2) = (uint32_t)gdt_content;

  _gdt_set_gdtr();
  _gdt_reload_segments();
}
