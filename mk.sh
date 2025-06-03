#!/bin/sh
set -e
nasm --reproducible -o os.bin os.nasm && ndisasm os.bin | grep -v "  0000 "
[ "$1" = "run" ] && qemu-system-i386 -fda os.bin -nographic -boot a -serial mon:stdio
[ "$1" = "debug" ] && blinkenlights -rtR os.bin