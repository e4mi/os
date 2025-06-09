ID := os
.PHONY: all clean run debug
all: os.bin
clean:
	rm -f os.bin
docker:
	docker build -t $(ID) .
	docker run -it --rm -v ./:/src -w /src $(ID)
os.bin: boot.nasm os.c
	./cc os.c -o os1.bin
	nasm -f bin boot.nasm -o os.bin -DFILE=\"os1.bin\" -DSIZE=$$(stat -c%s os1.bin)
	rm os1.bin
	wc -c os.bin
run: os.bin
	qemu-system-i386 -fda os.bin -boot a -nographic
debug: os.bin
	emulator -rt ./os.bin