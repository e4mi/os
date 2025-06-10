ID := os
CC := gcc
CFLAGS = -m16 -ffreestanding -nostdlib -nostartfiles -fno-pic -fno-pie -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=c89 -pedantic -Wall -Wextra -fomit-frame-pointer
SIZE = $(shell expr 4 \* 1024)
.PHONY: all clean docker run debug
all: os.bin
clean:
	rm -f os.bin
docker:
	docker build -t $(ID) .
	docker run -it --rm -v ./:/src -w /src $(ID)
os.bin: main.c lang.c libc_i86.asm libc.c
	$(CC) $(CFLAGS) -c main.c
	nasm -D SIZE=$(SIZE) -f elf32 libc_i86.asm -o libc_i86.o
	# ld -m elf_i386 -N -Ttext 0x7c00 -o os.bin libc_i86.o main.o
	ld -melf_i386 -T linker.ld -nostdlib libc_i86.o main.o -o os.bin
	wc -c os.bin
	test "$(SIZE)" -ge "$$(cat os.bin | wc -c)" || (echo "TOO BIG"; exit 1)
	truncate -s $(SIZE) os.bin
run: os.bin
	qemu-system-i386 -fda os.bin -boot a -nographic
debug: os.bin
	emulator -rt ./os.bin