ID := os
CC := gcc
CFLAGS = -m16 -ffreestanding -fno-pic -Os -std=c99 -pedantic -Wall
LDFLAGS = -T linker.ld
SIZE = $(shell expr 8 \* 1024)
.PHONY: all clean docker run debug
all: os.bin
	rm -f libc_i86.o os.o
clean:
	rm -f os.bin libc_i86.o os.o
docker:
	docker build -t $(ID) .
	docker run -it --rm -v ./:/src -w /src $(ID)
os.o: os.c lang.c libc.c
	$(CC) $(CFLAGS) -c os.c -o os.o
libc_i86.o: libc_i86.asm
	nasm -D SIZE=$(SIZE) -f elf32 libc_i86.asm -o libc_i86.o
os.bin: os.o libc_i86.o linker.ld
	$(LD) $(LDFLAGS) libc_i86.o os.o -o os.bin
	wc -c os.bin
	@test "$(SIZE)" -ge "$$(cat os.bin | wc -c)" || (echo "TOO BIG!"; exit 1)
	truncate -s $(SIZE) os.bin
run: os.bin
	qemu-system-i386 -fda os.bin -boot a -nographic
debug: os.bin
	emulator -rt os.bin