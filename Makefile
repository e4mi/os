ID := os
CC := gcc
CFLAGS = -m16 -ffreestanding -nostdlib -nostartfiles -fno-pic -fno-pie -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=c89 -pedantic -Wall -Wextra -Werror
SIZE = $(shell expr 8 \* 1024)
.PHONY: all clean docker run debug
all: os.bin
clean:
	rm -f os.bin libc_i86.o main.o
docker:
	docker build -t $(ID) .
	docker run -it --rm -v ./:/src -w /src $(ID)
os.bin: src/main.c src/lang.c src/libc_i86.asm src/libc.c
	$(CC) $(CFLAGS) -c src/main.c -o main.o
	nasm -D SIZE=$(SIZE) -f elf32 src/libc_i86.asm -o libc_i86.o
	ld -melf_i386 -T linker.ld -nostdlib libc_i86.o main.o -o os.bin
	wc -c os.bin
	test "$(SIZE)" -ge "$$(cat os.bin | wc -c)" || (echo "TOO BIG"; exit 1)
	truncate -s $(SIZE) os.bin
run: os.bin
	qemu-system-i386 -fda os.bin -boot a -nographic
debug: os.bin
	emulator -rt os.bin