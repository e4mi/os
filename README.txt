TINY SELF-HOSTED OS:
- bootstrapped from NASM 8086 assembly
- bootloader loads code from floppy
- code interpreter runs it

INSTALL:
make         # compiles code in docker
make run     # runs code in qemu
make debug   # runs code in blinkenlights debugger

LANGUAGE (todo):
(asm ... c3) ; machine code
(...)        ; create a function
!            ; calls function
123          ; push value
$emit        ; saves value to variable emit
^emit        ; get address of variable
emit         ; calls function