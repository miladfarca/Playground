#include "../include/kernel.h"
#include "../include/gdt.h"
#include "../include/idt.h"
#include "../include/memory.h"
#include "../include/pic.h"
#include "../include/shell.h"
#include "../include/version.h"
#include "../include/vga.h"
#include <stdint.h>

void kernel_init(void) {
  // GDT
  vga_print_ln("Setting GDT");
  gdt_set();

  // IDT
  vga_print_ln("Setting IDT");
  idt_set();

  // PIC
  vga_print_ln("Setting PIC");
  pic_set();

  // Interrupts
  vga_print_ln("Enabling interrupts");
  idt_enable_interrupts();
}

void kernel_init_shell(void) {
  shell_reset();
  shell_init();
}

void kernel_panic(char *msg, int interrupt_number) {
  vga_print_ln("");
  vga_print_s("Panic: ");
  if (interrupt_number >= 0) {
    vga_print_s("caught interrupt number ");
    vga_print_i(interrupt_number);
    vga_print_s(", ");
  }
  vga_print_ln(msg);
  vga_print_ln("Resetting kernel ...");
  _kernel_reset();
}

void kernel_main(void) {

  vga_clear();
  kernel_init();

  vga_print_s("Kernel starts at ");
  vga_print_i(memory_get_kernel_start());
  vga_print_ln("");

  vga_print_s("Kernel ends at ");
  vga_print_i(memory_get_kernel_end());
  vga_print_ln("");

  vga_print_ln("");
  vga_print_ln("");
  VERSION_PRINT_PLAYGROUND()
  vga_print_ln("");
  vga_print_ln("");
  vga_print_s("Welcome to Playground version ");
  vga_print_s(VERSION_STRING);
  vga_print_ln(".");
  vga_print_ln("Type 'help' to see a list of commands.");
  vga_print_ln(
      "Visit https://github.com/miladfarca/playground for more information.");
  vga_print_ln("Disclaimer: It is possible to damage your machine running "
               "certain instructions. Use at your own risk.");
  vga_print_ln("");
  vga_print_ln("");

  kernel_init_shell();

  // At this point we will hit an infinite loop defined in `boot.S` until
  // an interrupt occurs.
}
