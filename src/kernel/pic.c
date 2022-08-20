#include "../include/pic.h"
#include "../include/hal.h"
#include <stdint.h>

// https://wiki.osdev.org/PIC
// https://github.com/levex/osdev/blob/master/kernel/pic.c

void pic_set() {
  hal_outb(PIC_1_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
  hal_outb(PIC_2_COMMAND, PIC_ICW1_INIT | PIC_ICW1_ICW4);
  hal_outb(PIC_1_DATA, 0x20); // ICW2: Primary PIC vector offset
  hal_outb(PIC_2_DATA, 0x28); // ICW2: Secondary PIC vector offset
  hal_outb(PIC_1_DATA, 4); // ICW3: tell Primary PIC that there is a Secondary
                           // PIC at IRQ2 (0000 0100)
  hal_outb(PIC_2_DATA,
           2); // ICW3: tell Secondary PIC its cascade identity (0000 0010)

  hal_outb(PIC_1_DATA, PIC_ICW4_8086);
  hal_outb(PIC_2_DATA, PIC_ICW4_8086);
}
