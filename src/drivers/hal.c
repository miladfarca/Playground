#include "../include/hal.h"
#include <stdint.h>

void hal_outb(uint16_t portid, uint8_t value) {
  asm volatile("outb %%al, %%dx" : : "d"(portid), "a"(value));
}

uint8_t hal_inb(uint16_t portid) {
  uint8_t ret;
  asm volatile("inb %%dx, %%al" : "=a"(ret) : "d"(portid));
  return ret;
}
