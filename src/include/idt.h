#ifndef _IDT_H
#define _IDT_H
#include "idt_exceptions.h"
#include <stdint.h>

void idt_set_interrupt(uint8_t i, void (*callback)(void));
void idt_enable_interrupts();
void idt_set();
void _idt_set_idtr();
void _idt_default_isr();
char *idt_get_interrupt_description(int interrupt_number);

// generic wrappers.
#define IDT_EXCEPTION_WRAPPER(interrupt_number)                                \
  void _idt_exception_wrapper_##interrupt_number(void);
IDT_EXCEPTION_LIST(IDT_EXCEPTION_WRAPPER)
#undef IDT_EXCEPTION_WRAPPER

// specific isr wrappers.
void _idt_keyboard_isr_wrapper();

#endif
