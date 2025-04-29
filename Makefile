ID=os140534
RUN=docker run --rm -it -v ./:/app -w /app $(ID)

all: os.bin

clean:
	$(RUN) rm -f os.bin

os.bin: os.nasm os.txt
	$(RUN) nasm -f bin -o os.bin os.nasm
	$(RUN) dd if=os.txt of=os.bin bs=512 seek=1 count=1 conv=sync

docker: Dockerfile
	docker build -q -t $(ID) -f Dockerfile .

sh:
	$(RUN) sh

run: os.bin
	$(RUN) qemu-system-i386 -fda os.bin -nographic -boot a

debug: os.bin
	$(RUN) sh -c "blinkenlights -rt os.bin"

.PHONY: all clean docker sh run debug
