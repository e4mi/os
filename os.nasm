org 0x7c00

jmp start

%macro dpush 1
  mov ax, %1
  sub bp, 2
  mov [bp], ax
%endmacro

%macro dpop 0
  mov ax, [bp]
  add bp, 2
%endmacro

load_fda: ; ( addr sector -- addr )
  dpop
  mov cl, al
  dpop
  mov bx, ax
  xor dh, dh
  xor ch, ch
  mov dl, 0
  mov ah, 2
  mov al, 1
  int 13h
  dpush bx
  ret

goto_addr: ; ( addr -- )
  dpop
  jmp ax

emit: ; ( char -- )
  dpop
  mov ah, 0x0e
  int 10h
  cmp al, 10
  jne .nonl
  mov al, 13
  int 10h
  .nonl:
  ret

print_str: ; ( addr -- )
  dpop
  mov si, ax
.pls:
  lodsb
  or al, al
  jz .plend
  dpush ax
  call emit
  jmp .pls
.plend:
  ret

print_number: ; ( number -- )
  dpop
  cmp ax, 0
  jne .convert
  .zero:
    mov al, '0'
    call emit
    ret
  .convert:
    mov cx, 0
    mov bx, 10
  .digit_loop:
    xor dx, dx
    div bx ; ax = ax / 10, dx = ax % 10
    push dx
    inc cx
    or ax, ax
    jnz .digit_loop
  .print_loop:
    pop dx
    add dl, '0'
    mov al, dl
    xor ah, ah
    dpush ax
    call emit
    loop .print_loop
  ret


start:
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov bp, 0x9000

  dpush msg
  call print_str
  dpush 123
  call print_number
  dpush 10
  call emit

  ; dpush 0x0800
  ; dpush 2
  ; call load_fda
  ; call goto_addr

  hlt
  jmp $

msg: db 'meow ^^', 10, 0

times 510-($-$$) db 0
  dw 0xAA55
