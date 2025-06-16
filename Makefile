ID := os
CC := gcc
CFLAGS = -m16 -ffreestanding -fno-pic -Os -std=c89 -pedantic -Wall
LDFLAGS = -T linker.ld
SIZE = $(shell expr 8 \* 1024)
.PHONY: all clean docker run debug
all: os.bin
	rm -f os_8086.o os.o
clean:
	rm -f os.bin os_8086.o os.o
docker:
	docker build -t $(ID) .
	docker run -it --rm -v ./:/src -w /src $(ID)
os.o: os.c
	$(CC) $(CFLAGS) -c os.c -o os.o
os_8086.o: os_8086.asm
	nasm -D SIZE=$(SIZE) -f elf32 os_8086.asm -o os_8086.o
os.bin: os.o os_8086.o linker.ld
	$(LD) $(LDFLAGS) os_8086.o os.o -o os.bin
	wc -c os.bin
	@test "$(SIZE)" -ge "$$(cat os.bin | wc -c)" || (echo "TOO BIG!"; exit 1)
	truncate -s $(SIZE) os.bin
run: os.bin
	qemu-system-i386 -fda os.bin -boot a -nographic
debug: os.bin
	emulator -rt os.bin