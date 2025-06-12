ID := os
CC := gcc
CFLAGS = -m16 -ffreestanding -fno-pic -Os -std=c89 -pedantic -Wall
LDFLAGS = -T linker.ld
SIZE = $(shell expr 8 \* 1024)
.PHONY: all clean docker run debug
all: os.bin
	rm -f lib_i86.o main.o
clean:
	rm -f os.bin lib_i86.o main.o
docker:
	docker build -t $(ID) .
	docker run -it --rm -v ./:/src -w /src $(ID)
main.o: src/main.c src/dev.c src/lib.c src/types.c
	$(CC) $(CFLAGS) -c src/main.c -o main.o
lib_i86.o: src/lib_i86.asm
	nasm -D SIZE=$(SIZE) -f elf32 src/lib_i86.asm -o lib_i86.o
os.bin: main.o lib_i86.o
	$(LD) $(LDFLAGS) lib_i86.o main.o -o os.bin
	wc -c os.bin
	@test "$(SIZE)" -ge "$$(cat os.bin | wc -c)" || (echo "TOO BIG!"; exit 1)
	truncate -s $(SIZE) os.bin
run: os.bin
	qemu-system-i386 -fda os.bin -boot a -nographic
debug: os.bin
	emulator -rt os.bin