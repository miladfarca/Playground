#ifndef _HAL_H
#define _HAL_H
#include <stdint.h>

void hal_outb(uint16_t portid, uint8_t value);
uint8_t hal_inb(uint16_t portid);

#endif
