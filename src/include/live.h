#ifndef _LIVE_H
#define _LIVE_H

void live_draw_screen(int number_emitted_bytes);
void live_exec();
void live_init();

#define LIVE_REGISTERS(V)                                                      \
  V(EAX)                                                                       \
  V(ECX)                                                                       \
  V(EDX)                                                                       \
  V(EBX)                                                                       \
  V(ESP)                                                                       \
  V(EBP)                                                                       \
  V(ESI)                                                                       \
  V(EDI)                                                                       \
  V(SS)                                                                        \
  V(CS)                                                                        \
  V(DS)                                                                        \
  V(ES)                                                                        \
  V(FS)                                                                        \
  V(GS)

#define LIVE_RESTORABLE_REGISTERS(V)                                           \
  V(ECX)                                                                       \
  V(EDX)                                                                       \
  V(EBX)                                                                       \
  V(EBP)                                                                       \
  V(ESI)                                                                       \
  V(EDI)                                                                       \
  V(EAX)

#define LIVE_GENERATE_REGISTERS(V) int LIVE_REGISTER_##V;
LIVE_REGISTERS(LIVE_GENERATE_REGISTERS)
#undef LIVE_GENERATE_REGISTERS

// gcc is using `eax` to read/write a value from/to memory as such:
// movl LIVE_REGISTER_ECX(%eip), %eax
// movl %eax, %ECX
//
// We need to save EAX first and restore it last to avoid corrupting it.

#define LIVE_SAVE_REGISTERS(V)                                                 \
  asm("movl %%" #V ",%0" : "=r"(LIVE_REGISTER_##V));

#define LIVE_RESTORE_REGISTERS(V)                                              \
  asm("movl %0, %%" #V : : "r"(LIVE_REGISTER_##V));

#endif
