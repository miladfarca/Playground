#include "../include/idt.h"
#include "../include/idt_exceptions.h"
#include "../include/kernel.h"
#include "../include/memory_layout.h"
#include "../include/shell.h"
#include "../include/vga.h"
#include <stddef.h>
#include <stdint.h>

static uint16_t interrupt_count = 256; // in bytes.
static uint16_t idt_size = 0x800;
static uint8_t *idtr = (uint8_t *)MEMORY_IDTR_START;
static uint64_t *idt_content = (uint64_t *)MEMORY_IDT_START;

void idt_set_interrupt(uint8_t i, void (*callback)(void)) {
  uint32_t isr_address = (uint32_t)callback;
  uint16_t segment_selector = 0x8;
  uint64_t entry = ((isr_address & 0xFFFF0000) >> 16) * ((uint64_t)1 << 48) |
                   0x8E00 * ((uint64_t)1 << 32) |
                   segment_selector * ((uint64_t)1 << 16) |
                   (isr_address & 0xFFFF);
  *(idt_content + i) = entry;
}

void idt_enable_interrupts() { asm volatile("sti"); }

#define IDT_EXCEPTION_HANDLER(interrupt_number)                                \
  void idt_exception_hanlder_##interrupt_number(void) {                        \
    kernel_panic(idt_get_interrupt_description(interrupt_number),              \
                 interrupt_number);                                            \
  }
IDT_EXCEPTION_LIST(IDT_EXCEPTION_HANDLER)
#undef IDT_EXCEPTION_HANDLER

void idt_set() {
  // Memory layout:
  //
  // MEMORY_IDTR_START | 6 bytes (4 bytes address + 2 bytes limit (size -1))
  //                   |
  // MEMORY_IDT_START  | 8 bytes idt entry 0
  //                   |
  //                   | 8 bytes idt entry 1
  //                   | ...
  //
  // size is hard coded to be 256 entries.
  //
  // https://wiki.osdev.org/IDT
  // https://wiki.osdev.org/Segment_Selector

  // setup idtr content.
  *(uint16_t *)idtr = idt_size - 1;
  *(uint32_t *)(idtr + 2) = (uint32_t)idt_content;

// set exception handlers (first 32 interrupts).
#define IDT_EXCEPTION_HANLDER(interrupt_number)                                \
  idt_set_interrupt(interrupt_number,                                          \
                    &_idt_exception_wrapper_##interrupt_number);
  IDT_EXCEPTION_LIST(IDT_EXCEPTION_HANLDER)
#undef IDT_EXCEPTION_HANLDER

  // set all the other generic interrupt handlers.
  for (uint16_t i = 32; i < interrupt_count - 32; i++) {
    idt_set_interrupt(i, &_idt_default_isr);
  }

  // set keyboard handler.
  idt_set_interrupt(33, &_idt_keyboard_isr_wrapper);

  _idt_set_idtr();
}

char *idt_get_interrupt_description(int interrupt_number) {
  switch (interrupt_number) {
  case 0:
    return "Division by zero";
  case 1:
    return "Single-step interrupt";
  case 2:
    return "NMI";
  case 3:
    return "Breakpoint";
  case 4:
    return "Overflow";
  case 5:
    return "Bound Range Exceeded";
  case 6:
    return "Invalid Opcode";
  case 7:
    return "Coprocessor not available";
  case 8:
    return "Double Fault";
  case 9:
    return "Coprocessor Segment Overrun";
  case 10:
    return "Invalid Task State Segment";
  case 11:
    return "Segment not present";
  case 12:
    return "Stack Segment Fault";
  case 13:
    return "General Protection Fault";
  case 14:
    return "Page Fault";
  case 16:
    return "x87 Floating Point Exception";
  case 17:
    return "Alignment Check";
  case 18:
    return "Machine Check";
  case 19:
    return "SIMD Floating-Point Exception";
  case 20:
    return "Virtualization Exception";
  case 21:
    return "Control Protection Exception";
  }
  return NULL;
}
