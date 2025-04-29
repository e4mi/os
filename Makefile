ID=os140534
RUN=docker run --rm -it -v ./:/app -w /app $(ID)

os.bin: docker os.nasm
	$(RUN) nasm -f bin -o os.bin os.nasm

floppy.img: os.bin meow.app
	dd if=/dev/zero of=floppy.img bs=512 count=4
	dd if=os.bin of=floppy.img bs=512 count=2 conv=notrunc
	dd if=meow.app of=floppy.img bs=512 seek=2 conv=notrunc

docker: Dockerfile
	docker build -q -t $(ID) -f Dockerfile .

sh:
	$(RUN) sh

run: floppy.img
	$(RUN) qemu-system-i386 -fda floppy.img -nographic -boot a

debug: floppy.img
	$(RUN) sh -c "blinkenlights -rt floppy.img"

.PHONY: docker sh qemu debug
