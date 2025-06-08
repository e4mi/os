Tiny OS for 8086
- in NASM and ansi C
- compiles to a bootable floppy image
- bootable from qemu
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
