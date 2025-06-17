; NASM syntax
section .text
bits 16

extern main
global putchar
global exit
global clear
global getchar
global getkey
global _malloc_here
global _malloc_max

_bootloader:
.stack:
  cli
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, $$
  sti
.load:
  mov ax, 0x07E0
  mov es, ax
  mov ax, 0x0200 + ((SIZE + 511) / 512) - 1
  mov cx, 0x0002
  xor dx, dx
  xor bx, bx
  int 0x13
  jc .error
.start:
  call main
  jmp exit
.error:
  mov si, .msg
  call _print
  jmp exit
.msg:
  db 13, 10, ">_< meow?!", 13, 10, 0

_print:
.loop:
  lodsb
  test al, al
  jz .done
.emit:
  mov ah, 0x0e
  int 0x10
  cmp al, 10
  jne .loop
.nl:
  mov al, 13
  int 0x10
  jmp .loop
.done:
  ret

putchar:
  push bp
  mov bp, sp
  mov ax, [bp+6]
  mov ah, 0x0e
  int 0x10
  cmp al, 10
  jne .done
.nl:
  mov al, 13
  int 0x10
.done:
  pop bp
  ret

exit:
.loop:
  hlt
  jmp .loop

clear:
  mov ax, 0x0600
  mov bx, 0x0700
  xor cx, cx
  mov dx, 0x184F
  int 0x10
.cursor_home:
  mov ax, 0x0200
  xor bx, bx
  xor dx, dx
  int 0x10
  ret

getchar:
  push bp
.get:
  mov ah, 0x00
  int 0x16
  cmp al, 0
  je .get
  xor ah, ah
  cmp al, 13
  jne .done
.nl:
  mov al, 10
.done:
  pop bp
  ret

getkey:
  push bp
  mov ah, 0x00
  int 0x16
  pop bp
  ret

_malloc_here dw 0x0000, 0x0001
_malloc_max dw 0xFBFF, 0x0009

_signature:
  times 510 - ($ - $$) db 0
  dw 0xAA55
