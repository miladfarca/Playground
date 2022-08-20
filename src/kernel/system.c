#include "../include/system.h"
#include "../include/hal.h"
#include <stddef.h>

// https://wiki.osdev.org/CMOS
struct system_time system_get_time() {
#define BCD_TO_BINARY(val) ((val / 16) * 10) + (val & 0xf)
  // Status Register B, Bit 1 (value = 2): Enables 24 hour format if set
  // Status Register B, Bit 2 (value = 4): Enables Binary mode if set
  hal_outb(0x70, 0x0B);
  char sr = hal_inb(0x71);
  char _24hr_mode = sr & 0b010;
  char binary_mode = sr & 0b100;
  char *am_pm = NULL;

  hal_outb(0x70, 0x00);
  char s = hal_inb(0x71);
  if (!binary_mode)
    s = BCD_TO_BINARY(s);

  hal_outb(0x70, 0x02);
  char m = hal_inb(0x71);
  if (!binary_mode)
    m = BCD_TO_BINARY(m);

  hal_outb(0x70, 0x04);
  char h = hal_inb(0x71);
  if (!binary_mode)
    h = BCD_TO_BINARY(h);

  if (!_24hr_mode) {
    am_pm = "AM";
    // check (and clear) the 0x80 bit is set on the hour byte
    if (h & 0x80) {
      am_pm = "PM";
      h = h & 0x7F;
    }
  }

  return (struct system_time){s, m, h, am_pm};
}
