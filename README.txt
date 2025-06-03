tiny 8086 os (wip)

- fits in 512 bytes
- bootstrapped from NASM
- bootable from floppy

usage:
  nasm os.nasm -o os.bin
  qemu-system-i386 -fda os.bin

maybe:
- lisp-like language
- file system
- text editor