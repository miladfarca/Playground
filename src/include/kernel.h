#ifndef _KERNEL_H
#define _KERNEL_H

void _kernel_reset(void);
void kernel_init(void);
void kernel_init_shell(void);
void kernel_panic(char *msg, int interrupt_number);
void kernel_main(void);

#endif
