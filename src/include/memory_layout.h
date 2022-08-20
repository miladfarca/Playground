#ifndef _MEMORY_LAYOUT_H
#define _MEMORY_LAYOUT_H

// Kernel memory layout.
// Using the low 1 MiB region for BIOS:
// https://wiki.osdev.org/Memory_Map_(x86)
//
// High address  |
//               |
//               |
//   0x500010    |  free memory start (grows upward)
//               |
//   0x500001    |  live assembler start
//   0x500000    |  stack top (grows downward)
//               |
//     ^         |
//     |         |
//     | 3.9 MiB |
//     |         |
//     v         |
//               |
//   0x100830    | kernel start padded to 16 byte alignment
//               |
//   0x100824    | idt end
//               |
//     ^         |
//     |         |
//     | 2048 B  |
//     |         |
//     v         |
//               |
//   0x100024    | idt start
//               |
//   0x10001e    | idtr start
//               |
//     ^         |
//     |         |
//     | 24 B    |
//     |         |
//     v         |
//               |
//   0x100006    | gdt start
//               |
//   0x100000    | gdtr start
//               |
//     ^         |
//     |         |
//     | 1 Mib   | bios and vga text buffer
//     |         |
//     v         |
//               |
//     0x0       |

#define MEMORY_VGA_TEXT_BUFFER 0xB8000
#define MEMORY_GDTR_START 0x100000
#define MEMORY_GDT_START 0x100006
#define MEMORY_IDTR_START 0x10001e
#define MEMORY_IDT_START 0x100024
#define MEMORY_STACK_TOP 0x500000
#define MEMORY_LIVE_BUFFER_START 0x500001
#define MEMORY_FREE_START 0x500010

#endif
