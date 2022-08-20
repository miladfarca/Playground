#include "../include/memory_layout.h"
#include "../include/idt_exceptions.h" 

.section .text
.global _gdt_set_gdtr
.type _gdt_set_gdtr, @function
_gdt_set_gdtr:
	push %ebp
	movl %esp, %ebp

	lgdt MEMORY_GDTR_START

	movl %ebp, %esp
	pop %ebp
	ret

.global _gdt_reload_segments
.type _gdt_reload_segments, @function
_gdt_reload_segments:
	push %ebp
	movl %esp, %ebp

	push %eax
	mov $0x10, %ax
	mov %ax, %ds
	mov %ax, %es
	mov %ax, %fs
	mov %ax, %gs
	mov %ax, %ss
	pop %eax

	ljmp $0x8, $next
next:
	movl %ebp, %esp
	pop %ebp
	ret

.global _idt_set_idtr
.type _idt_set_idtr, @function
_idt_set_idtr:
   	push %ebp
   	movl %esp, %ebp

  	lidt MEMORY_IDTR_START 
   
   	movl %ebp, %esp
   	pop %ebp
   	ret

.globl _idt_keyboard_isr_wrapper
.type _idt_keyboard_isr_wrapper, @function
_idt_keyboard_isr_wrapper:
    	pushal
    	cld
    	call keyboard_isr
    	popal
    	iretl

# We cannot do an `iret` here and the last `call` should never return.
# An `iret` here will return back to the faulty instruction and we will
# execute this isr infinitely: https://forum.osdev.org/viewtopic.php?f=1&t=19438
#define _IDT_EXCEPTION_WRAPPER(interrupt_number)             \
.globl _idt_exception_wrapper_##interrupt_number;            \
.type _idt_exception_wrapper_##interrupt_number , @function; \
_idt_exception_wrapper_##interrupt_number:                   \
    	cld;                                                 \
	call idt_exception_hanlder_##interrupt_number;
IDT_EXCEPTION_LIST(_IDT_EXCEPTION_WRAPPER)
#undef _IDT_EXCEPTION_WRAPPER

.global _idt_default_isr
.type _idt_default_isr, @function
_idt_default_isr:
	pushal
	mov $0x20, %al
	mov $0x20, %dx
	out %al, %dx
	popal
	iretl

