org 0x7c00
jmp init

%macro CALL 2-*
  %rep %0
    mov %[%i], %[%i + 1]
    %assign i i + 2
  %endrep
  call %1
%endmacro

; free memory in 8086: 0x500 - 0xA0000
code_start equ 0x8000
heap_start equ 0x1000

vars dw 0
compiled dw 0
heap_end dw heap_start
stack dw 0

init:
  ; set up stack
  xor ax, ax
  mov ss, ax
  mov sp, $$

  call init_data
  call load_fda
  
  mov di, [compiled]
  mov si, code_start
  call parse

  call [compiled]

  jmp $

init_data:
  mov ax, 128 * 2
  call alloc
  mov [vars], ax

  mov ax, 1024
  call alloc
  mov [stack], ax

  mov ax, 1024
  call alloc
  mov [compiled], ax

  ret

load_fda:
  mov ah, 0x02 ; read
  mov al, 1 ; n sectors
  mov ch, 0 ; cylinder
  mov dh, 0 ; head
  mov dl, 0 ; fda
  mov cl, 2 ; sector
  mov bx, code_start / 16 ; destination
  mov es, bx
  xor bx, bx ; offset in es
  int 0x13
  ret


parse: ; (si: code, di: dest)
  lodsb
  .next:
    or al, al
    jz .done
    cmp al, ';'
    je .comment
    cmp al, ' '
    jbe parse
    cmp al, '#'
    je .hex
    cmp al, '$'
    jbe .set
    cmp al, '!'
    jbe .apply
    cmp al, 'z'
    jbe .symbol
    jmp parse
  .comment:
    lodsb
    or al, al
    jz .done
    cmp al, 10
    jne .comment
    jmp parse
  .hex:
    xor bx, bx
    xor cx, cx
  .hex_loop:
    lodsb
    or al, al
    jz .done
    sub al, '0'
    cmp al, 9
    jbe .store_digit
    sub al, 'a' - '0' - 10
    cmp al, 0xf
    ja .hex_done
  .store_digit:
    shl bx, 4
    or bx, ax
    inc cx
    jmp .hex_loop
  .hex_done:
    mov [di], bx
    inc di
    jmp .next
  .set:
    jmp parse
  .apply:
    jmp parse
  .symbol:
    jmp parse
  .done:
    ret

alloc: ; (ax: bytes)
  mov bx, [heap_end]
  add word [heap_end], ax
  mov ax, bx
  ret

second_push: ; (ax: value)
  mov bx, [stack]
  sub bx, 2
  mov [bx], ax
  mov [stack], bx
  ret

second_pop: ; () => (ax: value)
  mov bx, [stack]
  mov ax, [bx]
  add bx, 2
  mov [stack], bx
  ret

signature:
  times 510 - ($ - $$) db 0
  dw 0xAA55
