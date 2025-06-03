tiny 8086 os (wip)

- fits in 512 bytes
- bootstrapped from NASM
- bootable from floppy

usage:
  nasm os.nasm
  qemu-system-i386 -fda os

maybe:
- lisp-like language
- file system
- text editor