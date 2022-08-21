## What is Playground?
Playground is a small Monolithic 32-bit Kernel which you can boot on an x86-64 machine and gain access to nearly all
the resources of your system. Playground runs fully in kernel mode (ring 0 on x86) and gives you access to up to 4GB of physical memory.
There is no usage of paging or virtual memory, you get full access to your physical RAM, ports and privileged CPU instructions.

It's a great tool for exploring bare metal x86 hardware or use it as a base for your own projects.

## What operations am I allowed to perform?
Anything your hardware allows you to, here are some examples:

- Write to any address of your physical memory and read it back.
- Write directly to your VGA buffer and see your input characters appear on your screen.
- Use IN/OUT instructions to access all your ports such as CMOS registers, Serial ports, Motherboard speakers etc.
- Overwrite the kernel itself (this will probably cause a triple fault and reboot your machine).

You can also define your own functions or commands, re-build the project and run your code independent of an operating system.
Note that there is no `libc` available, all there is is your own code. A set of helper functions is available under `src/utils/utils.c`.

## Features
Playground provides to following basic features:
- Simple memory management
- Interrupt handling
- Basic IO management (display and keyboard)

It also includes a shell and builtin programs, use `help` to see a list of them.

## live assembler
`live` is a builtin program which allows you to run one x86 instruction at a time.
It also displays the content of a set of registers which gets updated every time you run an instruction.
Instructions are expected to be in Intel syntax. Supported opcodes are listed under `src/builtins/live/opcodes.h`.

Note that `live` is still in development, not all opcodes are currently functional.
A list of sample instructions is available under `src/builtins/live/sample.S`.

## Disclaimer
This project is currently in beta phase which means there are many bugs and unfinished features in the code.
Due to lack of any security layers you are absolutely able to overwrite some vital registers and render your system unusable.

Use at your own risk.

## Build and Usage
Prebuilt ISO images are available under *Releases*.
You can burn the image on a USB stick or CD and boot it on an x86-64 hardware. You can also use Virtual Box or QEMU.

`./build` can be used to compile the project and generate an ISO image, build tools include the following:

- `i386-elf` compiler toolchain which can be found at the bottom of this page: https://wiki.osdev.org/GCC_Cross-Compiler
- `GRUB` bootloader which is used to build the final bootable ISO image:
```
apt install grub-pc-bin xorriso
```
- `QEMU` which is needed for local execution:
```
apt install qemu-system-x86
```
`./run` can be used to run the ISO file using `QEMU`.
