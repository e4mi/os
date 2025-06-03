tiny 8086 os (wip)

features:
- fits in 512 bytes
- bootstrapped from NASM
- bootable from floppy

todo:
- forth-like interpreter
- lisp-like interpreter
- builds itself on the fly

usage:
  nasm os.nasm -o os.bin
  qemu-system-i386 -fda os.bin
