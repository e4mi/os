Tiny OS for 8086
- in C99
- compiles to a bootable floppy image
- runs in qemu
- optional docker for building

INSTALL:
  make
  make run

DEPS: gcc make nasm
OPTIONAL: qemu-system-i386 blinkenlights

WISHLIST:
- own lisp-like language
- filesystem
- editor
