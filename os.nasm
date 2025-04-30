org 0x7c00
jmp init
; free memory in 8086: 0x500 - 0xA0000
code_start equ 0x8000
heap_start equ 0x1000

vars dw 0
compiled dw 0
heap_end dw heap_start
stack dw 0

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
  mov ax, 128 * 2 ; bytes
  call alloc
  mov [vars], ax

  mov ax, 1024 ; bytes
  call alloc
  mov [stack], ax

  mov ax, 1024
  call alloc
  mov [compiled], ax
  
  mov di, ax
  mov si, code_start
  call parse

  call [compiled]
  ret

parse:
  xor bx, bx
  xor cx, cx
  .next:
    lodsb
    or al, al
    jz .done
    cmp al, ';'
    je .comment
    cmp al, ' '
    jbe .next
    cmp al, '#'
    je .hex
    cmp al, '$'
    jbe .set
    cmp al, '!'
    jbe .apply
    cmp al, 'z'
    jbe .symbol
    jmp .next
  .comment:
    lodsb
    or al, al
    jz .done
    cmp al, 10
    jne .comment
    jmp .next
  .hex:
  .hex1:
    lodsb
    or al, al
    jz .done
    sub al, '0'
    cmp al, 'a'
    jb .hex1a
    sub al, 'a' - '0'
  .hex1a:
    mov bx, ax
    shl bx, 4
  .hex2:
    lodsb
    or al, al
    jz .done
    sub al, '0'
    cmp al, 'a'
    jb .hex2a
    sub al, 'a' - '0'
  .hex2a:
    or bx, ax
  .hex_write:
    mov [di], bx
    inc di
    jmp .next
  .symbol:
  .set:
  .apply:
  .done:
    ret

alloc:
  mov bx, [heap_end]
  add word [heap_end], ax
  mov ax, bx
  ret

signature:
  times 510 - ($ - $$) db 0
  dw 0xAA55
