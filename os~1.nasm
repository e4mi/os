org 0x7c00
jmp init

; free memory in 8086: 0x500 - 0xA0000
; align 16
vars dw 0
source dw 0
compiled dw 0
heap dw 0x1000
stack dw 0
stack_size equ 1024

init:
  xor ax, ax
  mov ss, ax
  mov sp, $$

start:
  call init_data

  mov ax, [source]
  call load_fda
  
  mov di, [compiled]
  mov si, [source]
  call parse

  call [compiled]

  jmp $

init_data:
  mov ax, 128 * 2
  call alloc
  mov [vars], ax

  mov ax, stack_size
  call alloc
  mov [stack], ax
  mov bp, ax
  add bp, stack_size

  mov ax, 1024
  call alloc
  mov [compiled], ax

  mov ax, 4 * 1024
  call alloc
  mov [source], ax

  ret

init_functions:
  mov ax, [vars]
  mov bx, ax
  add bx, ';'
  mov [bx], comment

var: ; ( symbol -- address )
  mov bx, [vars]
  dpush
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
  dpush: ; () => (ax: value)
  mov ax, [bp]
  add bp, 2
  ret

signature:
  times 510 - ($ - $$) db 0
  dw 0xAA55
