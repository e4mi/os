ID := os
CC := gcc
CFLAGS = -m16 -ffreestanding -fno-pic -Os -std=c89 -pedantic -Wall
LDFLAGS = -T linker.ld
SIZE = $(shell expr 8 \* 1024)
.PHONY: all clean docker run debug
all: os.bin
	rm -f libc_i86.o main.o
clean:
	rm -f os.bin libc_i86.o main.o
docker:
	docker build -t $(ID) .
	docker run -it --rm -v ./:/src -w /src $(ID)
main.o: src/main.c src/dev.c src/libc.c src/nice.c
	$(CC) $(CFLAGS) -c src/main.c -o main.o
libc_i86.o: src/libc_i86.asm
	nasm -D SIZE=$(SIZE) -f elf32 src/libc_i86.asm -o libc_i86.o
os.bin: main.o libc_i86.o
	$(LD) $(LDFLAGS) libc_i86.o main.o -o os.bin
	wc -c os.bin
	@test "$(SIZE)" -ge "$$(cat os.bin | wc -c)" || (echo "TOO BIG!"; exit 1)
	truncate -s $(SIZE) os.bin
run: os.bin
	qemu-system-i386 -fda os.bin -boot a -nographic
debug: os.bin
	emulator -rt os.bin