os.bin: os.nasm
	nasm --reproducible -o os.bin os.nasm
	ndisasm os.bin | grep -v "  0000 "
run: os.bin
	qemu-system-i386 -fda os.bin -nographic -boot a -serial mon:stdio
debug: os.bin
	blinkenlights -rtR os.bin
clean:
	rm -f os.bin