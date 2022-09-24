mkdir -p obj
# compile src
CC="i386-elf-gcc"
CFLAGS="-std=gnu99 -ffreestanding -O2 -Wall -Wextra"
$CC -c src/boot.S -o obj/boot.o
$CC -c src/kernel/kernel_as.S -o obj/kernel_as.o
$CC -c src/kernel/kernel.c -o obj/kernel.o $CFLAGS
$CC -c src/kernel/gdt.c -o obj/gdt.o $CFLAGS
$CC -c src/kernel/idt.c -o obj/idt.o $CFLAGS
$CC -c src/kernel/pic.c -o obj/pic.o $CFLAGS
$CC -c src/kernel/system.c -o obj/system.o $CFLAGS
$CC -c src/kernel/memory.c -o obj/memory.o $CFLAGS
$CC -c src/utils/utils.c -o obj/utils.o $CFLAGS
$CC -c src/drivers/vga.c -o obj/vga.o $CFLAGS
$CC -c src/drivers/hal.c -o obj/hal.o $CFLAGS
$CC -c src/drivers/keyboard.c -o obj/keyboard.o $CFLAGS
$CC -c src/builtins/builtins.c -o obj/builtins.o $CFLAGS
$CC -c src/builtins/shell.c -o obj/shell.o $CFLAGS
$CC -c src/builtins/live/assembler.c -o obj/assembler.o $CFLAGS
$CC -c src/builtins/live/live.c -o obj/live.o $CFLAGS

# link
$CC -T src/linker.ld -o obj/kernel.bin -ffreestanding -nostdlib obj/boot.o obj/kernel_as.o obj/kernel.o obj/gdt.o obj/idt.o obj/pic.o obj/system.o obj/memory.o obj/utils.o obj/hal.o obj/keyboard.o obj/vga.o obj/builtins.o obj/shell.o obj/assembler.o obj/live.o -lgcc

# build ISO
# can be written to a USB drive using:
# dd if=playground.iso of=/dev/<device name> && sync
mkdir -p isodir/boot/grub
cp obj/kernel.bin isodir/boot/playground.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o playground.iso isodir
