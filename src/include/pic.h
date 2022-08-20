#ifndef _PIC_H
#define _PIC_H

#define PIC_ICW1_ICW4 0x01 /* ICW4 (not) needed */
#define PIC_ICW1_INIT 0x10 /* Initialization - required! */
#define PIC_ICW4_8086 0x01 /* 8086/88 (MCS-80/85) mode */
#define PIC_1_COMMAND 0x20
#define PIC_1_DATA 0x21
#define PIC_2_COMMAND 0xA0
#define PIC_2_DATA 0xA1
#define PIC_COMMAND_EOI 0x20

void pic_set();

#endif
