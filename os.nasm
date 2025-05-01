org 0x7c00
jmp init

%macro put 1-*
  %assign i 1
  %rep %0
    mov ax, %i
    sub bp, 2
    mov [bp], ax
    %assign i i+1
  %endrep
%endmacro

%macro do 1-*
  %assign i 2
  %rep %0 - 1
    mov ax, %i
    sub bp, 2
    mov [bp], ax
    %assign i i+1
  %endrep
  call %1
%endmacro

%macro take 0
  mov ax, [bp]
  add bp, 2
%endmacro

%macro data 2
  mov ax, %2
  call alloc
  mov [%1], ax
%endmacro

%macro var 1
  %1 dw 0
%endmacro

alloc: ; (size)
  mov bx, [heap]
  take
  add word [heap], ax
  put bx
  ret

; free memory in 8086: 0x500 - 0xA0000
; align 16
init:
  stack_size equ 1024
  heap dw 0x1000
  
  var vars
  do alloc, vars, 256

  var source
  do alloc, source, 4096
  var compiled
  do alloc, compiled, 1024
  var stack
  do alloc, stack, 1024
  
  xor ax, ax
  mov ss, ax
  mov sp, $$
  mov bp, [stack]
  add bp, stack_size

start:
  call init_data

  mov ax, [source]
  call load_fda
  
  mov di, [compiled]
  mov si, [source]
  call parse

  call [compiled]

  jmp $


init_functions:
  put comment
  put ';'
  var
  set

var: ; ( symbol -- address )
  mov bx, [vars]
  call take
  dpush [vars]
  add ax, bx
  dpush
  ret

set: ; ( symbol value -- )
  mov bx, [vars]
  dpush
  add bx, ax
  dpush
  mov [bx], ax
  ret

load_fda: ; (ax: destination)
  mov ax, es
  mov bx, 16
  div bl ; destination segment
  mov es, ax
  mov ah, 0x02 ; read
  mov al, 1 ; n sectors
  mov ch, 0 ; cylinder
  mov dh, 0 ; head
  mov dl, 0 ; fda
  mov cl, 2 ; sector
  xor bx, bx ; offset in es
  int 0x13
  ret


parse: ; (si: code, di: dest)
  .read_next:
    lodsb
  .next:
    or al, al
    jz .done
    cmp al, ';'
    je .comment
    cmp al, ' '
    jbe .read_next
    cmp al, '#'
    je .hex
    cmp al, '$'
    jbe .set
    cmp al, '!'
    jbe .apply
    cmp al, 'z'
    jbe .symbol
    jmp .read_next
  .comment:
    lodsb
    or al, al
    jz .done
    cmp al, 10
    jne .comment
    jmp .read_next
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
    jmp .read_next
  .apply:
    jmp .read_next
  .symbol:
    xor ah, ah
    mov bx, [vars]
    add ax, bx
    call dpush
    jmp .read_next
  .done:
    ret

alloc: ; (ax: bytes)
  mov bx, [heap]
  add word [heap], ax
  mov ax, bx
  ret

dpush: ; (ax: value)
  sub bp, 2
  mov [bp], ax
  ret

dpop: ; () => (ax: value)
  mov ax, [bp]
  add bp, 2
  ret

signature:
  times 510 - ($ - $$) db 0
  dw 0xAA55
