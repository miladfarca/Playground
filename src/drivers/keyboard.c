#include "../include/keyboard.h"
#include "../include/gdt.h"
#include "../include/hal.h"
#include "../include/idt.h"
#include "../include/pic.h"
#include "../include/shell.h"
#include "../include/vga.h"
#include <stdint.h>

static char is_shift_pressed = 0;

// https://github.com/szhou42/osdev/blob/master/src/kernel/drivers/keyboard.c
static char keys_no_shift[128] = {
    0,    27,  '1', '2', '3',  '4', '5', '6', '7',  '8', /* 9 */
    '9',  '0', '-', '=', '\b',                           /* Backspace */
    '\t',                                                /* Tab */
    'q',  'w', 'e', 'r',                                 /* 19 */
    't',  'y', 'u', 'i', 'o',  'p', '[', ']', '\n',      /* Enter key */
    0,                                                   /* 29   - Control */
    'a',  's', 'd', 'f', 'g',  'h', 'j', 'k', 'l',  ';', /* 39 */
    '\'', '`', 0,                                        /* Left shift */
    '\\', 'z', 'x', 'c', 'v',  'b', 'n',                 /* 49 */
    'm',  ',', '.', '/', 0,                              /* Right shift */
    '*',  0,                                             /* Alt */
    ' ',                                                 /* Space bar */
    0,                                                   /* Caps lock */
    0,                                                   /* 59 - F1 key ... > */
    0,    0,   0,   0,   0,    0,   0,   0,   0,         /* < ... F10 */
    0,                                                   /* 69 - Num lock*/
    0,                                                   /* Scroll Lock */
    0,                                                   /* Home key */
    0,                                                   /* Up Arrow */
    0,                                                   /* Page Up */
    '-',  0,                                             /* Left Arrow */
    0,    0,                                             /* Right Arrow */
    '+',  0,                                             /* 79 - End key*/
    0,                                                   /* Down Arrow */
    0,                                                   /* Page Down */
    0,                                                   /* Insert Key */
    0,                                                   /* Delete Key */
    0,    0,   0,   0,                                   /* F11 Key */
    0,                                                   /* F12 Key */
    0, /* All other keys are undefined */
};

static char keys_with_shift[128] = {
    0,    27,  '!', '@', '#',  '$', '%', '^', '&',  '*', /* 9 */
    '(',  ')', '_', '+', '\b',                           /* Backspace */
    '\t',                                                /* Tab */
    'Q',  'W', 'E', 'R',                                 /* 19 */
    'T',  'Y', 'U', 'I', 'O',  'P', '{', '}', '\n',      /* Enter key */
    0,                                                   /* 29   - Control */
    'A',  'S', 'D', 'F', 'G',  'H', 'J', 'K', 'L',  ':', /* 39 */
    '"',  '~', 0,                                        /* Left shift */
    '|',  'Z', 'X', 'C', 'V',  'B', 'N',                 /* 49 */
    'M',  '<', '>', '?', 0,                              /* Right shift */
    '*',  0,                                             /* Alt */
    ' ',                                                 /* Space bar */
    0,                                                   /* Caps lock */
    0,                                                   /* 59 - F1 key ... > */
    0,    0,   0,   0,   0,    0,   0,   0,   0,         /* < ... F10 */
    0,                                                   /* 69 - Num lock*/
    0,                                                   /* Scroll Lock */
    0,                                                   /* Home key */
    0,                                                   /* Up Arrow */
    0,                                                   /* Page Up */
    '-',  0,                                             /* Left Arrow */
    0,    0,                                             /* Right Arrow */
    '+',  0,                                             /* 79 - End key*/
    0,                                                   /* Down Arrow */
    0,                                                   /* Page Down */
    0,                                                   /* Insert Key */
    0,                                                   /* Delete Key */
    0,    0,   0,   0,                                   /* F11 Key */
    0,                                                   /* F12 Key */
    0, /* All other keys are undefined */
};

void keyboard_to_ascii(uint8_t key) {

  if (key == KEYBOARD_ESCAPE_PRESSED) {
    shell_restart();
    return;
  } else if (key == KEYBOARD_ENTER_PRESSED) {
    shell_parse_command();
    return;
  } else if (key == KEYBOARD_BACKSPACE_PRESSED) {
    vga_print_back_space();
    return;
  } else if (key == KEYBOARD_LEFT_SHIFT_PRESSED ||
             key == KEYBOARD_RIGHT_SHIFT_PRESSED) {
    is_shift_pressed = 1;
    return;
  } else if (key == KEYBOARD_LEFT_SHIFT_RELEASED ||
             key == KEYBOARD_RIGHT_SHIFT_RELEASED) {
    is_shift_pressed = 0;
    return;
  }

  if (key < 128)
    vga_print_char(is_shift_pressed ? keys_with_shift[key]
                                    : keys_no_shift[key]);
}

void keyboard_isr() {
  keyboard_to_ascii(hal_inb(0x60));
  hal_outb(PIC_1_COMMAND, PIC_COMMAND_EOI);
}
