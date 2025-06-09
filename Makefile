ID := os
CC := gcc
CFLAGS = -m16 -ffreestanding -nostdlib -nostartfiles -fno-pic -fno-pie -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -std=c99
.PHONY: all clean docker run debug
all: os.bin
clean:
	rm -f os.bin
docker:
	docker build -t $(ID) .
	docker run -it --rm -v ./:/src -w /src $(ID)
os.bin: os.c os.ld
	$(CC) $(CFLAGS) -Wl,-T,os.ld -o os.bin os.c
	wc -c os.bin
run: os.bin
	qemu-system-i386 -fda os.bin -boot a -nographic
debug: os.bin
	emulator -rt ./os.bin