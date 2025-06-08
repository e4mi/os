CC := gcc
CFLAGS := -Os -static -m16 -ffreestanding -fno-pic -fno-pie
.PHONY: all clean
all: floppy.bin
clean:
	rm -f kernel.bin floppy.bin
kernel.bin: kernel.c io_8086.c
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o
	ld -T link.ld -m elf_i386 -nostdlib -o kernel.bin kernel.o
	rm kernel.o
floppy.bin: boot.nasm kernel.bin
	nasm -f bin boot.nasm -o floppy.bin \
		-DKERNEL_FILE=\"kernel.bin\" \
		-DKERNEL_SIZE=$(shell stat -c%s kernel.bin)
run: floppy.bin
	qemu-system-i386 -fda floppy.bin -boot a -nographic
debug: floppy.bin
	emulator -rt ./floppy.bin