ID := os
.PHONY: all clean run debug
all: os.bin
clean:
	rm -f os.bin
docker:
	docker build -t $(ID) .
	docker run -it --rm -v ./:/src -w /src $(ID)
os.bin: bin/boot.nasm bin/kernel.c lib/os.c lib/os_i86.c lib/file.c
	./cc bin/kernel.c -o kernel.bin
	nasm -f bin bin/boot.nasm -o os.bin -DFILE=\"kernel.bin\" -DSIZE=$$(stat -c%s kernel.bin)
	rm kernel.bin
	wc -c os.bin
run: os.bin
	qemu-system-i386 -fda os.bin -boot a -nographic
debug: os.bin
	emulator -rt ./os.bin