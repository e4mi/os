#!/bin/sh
# compile to flat binary using gcc
set -e
echo '
	OUTPUT_FORMAT("binary");
	OUTPUT_ARCH(i386);
	ENTRY(_start);
	SECTIONS {
		. = 0x7e00;
		.text : { *(.text*) }
	}
' > .cc.ld
trap 'rm .cc.ld' EXIT
gcc -static -m16 -ffreestanding -fno-pic -fno-pie -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,-T,.cc.ld -nostdlib -nostartfiles -s -fomit-frame-pointer --std=c89 -pedantic "$@"
