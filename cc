#!/bin/sh
# compile to flat binary using gcc
set -e
TMP=$(mktemp)
trap 'rm -f $TMP' EXIT
echo '
	OUTPUT_FORMAT("binary");
	OUTPUT_ARCH(i386);
	SECTIONS {
		. = 0x7e00;
		.text : { *(.text*) }
	}
' > ${TMP}
gcc -static -m16 -ffreestanding -fno-pic -fno-pie -Os -ffunction-sections -fdata-sections -Wl,--gc-sections -Wl,-T,${TMP} -nostdlib -nostartfiles -s -fomit-frame-pointer --std=c99 "$@"
