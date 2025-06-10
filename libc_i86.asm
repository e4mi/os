section .text
bits 16

extern main
global putchar
global printf
global exit
global clear
global getchar

bootloader:
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
.error:
  call .msg
  db 13, 10, ">_< meow?!", 13, 10, 0
.msg:
  push 0
  call printf
  jmp exit

putchar:
  push bp
  mov bp, sp
  mov ax, [bp+6]
  mov ah, 0x0e
  cmp al, 10
  je .nl
.char:
  int 0x10
  pop bp
  ret
.nl:
  mov al, 13
  int 0x10
  mov al, 10
  int 0x10
  pop bp
  ret

printf:
  push bp
  mov bp, sp
  mov si, [bp+6]
.loop:
  lodsb
  cmp al, 0
  je .done
  mov ah, 0x0e
  int 0x10
  cmp al, 10
  jne .loop
  mov al, 13
  int 0x10
  jmp .loop
.done:
  pop bp
  ret

exit:
.l:
  hlt
  jmp .l

clear:
  mov ax, 0x0600
  mov bx, 0x0700
  xor cx, cx
  mov dx, 0x184F
  int 0x10
  mov ax, 0x0200
  xor bx, bx
  xor dx, dx
  int 0x10
  ret

getchar:
  mov ah, 0x00
  int 0x16
  cmp al, 13
  jne .c
.nl:
  mov al, 10
.c:
  mov ah, 0
  ret

bootSignature:
  times 510 - ($ - $$) db 0
  dw 0xAA55
