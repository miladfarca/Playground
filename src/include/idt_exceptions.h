#ifndef _IDT_EXCEPTIONS_H
#define _IDT_EXCEPTIONS_H

// The IDT consists of 256 interrupt vectors–the first 32 (0–31 or 0x00–0x1F) of
// which are used for processor exceptions.
// https://en.wikipedia.org/wiki/Interrupt_descriptor_table
#define IDT_EXCEPTION_LIST(V)                                                  \
  V(0)                                                                         \
  V(1) V(2) V(3) V(4) V(5) V(6) V(7) V(8) V(9) V(10) V(11) V(12) V(13) V(14)   \
      V(15) V(16) V(17) V(18) V(19) V(20) V(21) V(22) V(23) V(24) V(25) V(26)  \
          V(27) V(28) V(29) V(30) V(31)

#endif
