org 0x7c00
code_start equ 0x8000
heap_start equ 0x9000

init:
  xor ax, ax
  mov ss, ax
  mov sp, $$

start:
  call load
  call run
  jmp $

load:
  mov ah, 0x02 ; read
  mov al, 1 ; n sectors
  mov ch, 0 ; cylinder
  mov dh, 0 ; head
  mov dl, 0 ; floppy
  mov cl, 2 ; sector
  mov bx, code_start / 16 ; destination
  mov es, bx
  xor bx, bx ; offset in es
  int 0x13
  ret

run:
  mov ax, 1024
  call alloc
  push ax
  mov di, ax
  mov si, code_start
  call parse
  pop ax
  call ax
  ret

parse:
  xor bx, bx
  xor cx, cx
  .next:
    lodsb
    or al, al
    jz .done
    cmp al, ' '
    je .next
    cmp al, '9'
    jbe .digit
    sub al, 'a' - 10
    jmp .hex
  .digit:
    sub al, '0'
  .hex:
    shl bx, 4
    or bl, al
    inc cx
    cmp cx, 2
    jne .next
    mov [di], bl
    inc di
    xor bx, bx
    xor cx, cx
    jmp .next
  .done:
    ret

alloc:
  mov bx, [heap_end]
  add word [heap_end], ax
  mov ax, bx
  ret

heap_end dw heap_start

signature:
  times 510 - ($ - $$) db 0
  dw 0xAA55
