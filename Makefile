ID=os140534
RUN=docker run --rm -it -v ./:/app -w /app $(ID)

os.bin: docker os.nasm
	$(RUN) nasm -f bin -o os.bin os.nasm

docker: Dockerfile
	docker build -q -t $(ID) -f Dockerfile .

sh:
	$(RUN) sh

run: os.bin
	$(RUN) qemu-system-i386 -fda os.bin -nographic -boot a

debug: os.bin
	$(RUN) sh -c "blinkenlights -rt os.bin"

.PHONY: docker sh qemu debug
