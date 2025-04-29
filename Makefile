ID=os140534
RUN=docker run --rm -it -v ./:/app -w /app $(ID)

os.bin: os_i86.nasm os.txt
	$(RUN) nasm -f bin -o os.bin os_i86.nasm
	dd if=/dev/zero of=os.bin bs=512 seek=1 count=1
	dd if=os.txt of=os.bin bs=512 seek=1 count=1 conv=notrunc

docker: Dockerfile
	docker build -q -t $(ID) -f Dockerfile .

sh:
	$(RUN) sh

run: os.bin
	$(RUN) qemu-system-i386 -fda os.bin -nographic -boot a

debug: os.bin
	$(RUN) sh -c "blinkenlights -rt os.bin"

.PHONY: docker sh qemu debug
