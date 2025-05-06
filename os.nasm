org 0x7c00

jmp start
dict dw 0
heap dw 0x9000

%macro _push 1
  mov ax, %1
  sub bp, 2
  mov [bp], ax
%endmacro

%macro _pop 0
  mov ax, [bp]
  add bp, 2
%endmacro

_load: ; ( addr sector -- addr )
  _pop
  mov cl, al
  _pop
  mov bx, ax
  xor dh, dh
  xor ch, ch
  mov dl, 0
  mov ah, 2
  mov al, 1
  int 13h
  _push bx
  ret

_goto: ; ( addr -- )
  _pop
  jmp ax

_emit: ; ( char -- )
  _pop
  mov ah, 0x0e
  int 10h
  cmp al, 10
  jne .nonl
  mov al, 13
  int 10h
  .nonl:
  ret

_print: ; ( addr -- )
  _pop
  mov si, ax
.pls:
  lodsb
  or al, al
  jz .plend
  _push ax
  call _emit
  jmp .pls
.plend:
  ret

_print_dec: ; ( number -- )
  _pop
  cmp ax, 0
  jne .convert
  .zero:
    mov al, '0'
    call _emit
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
    _push ax
    call _emit
    loop .print_loop
  ret

_same: ; ( addr1 addr2 -- addr1==addr2 )
  _pop
  mov si, ax
  _pop
  mov di, ax
  xor cx, cx
  .compare_loop:
    lodsb
    cmp al, [di]
    jne .not_equal
    or al, al
    jz .equal
    inc cx
    inc di
    jmp .compare_loop
  .not_equal:
    _push 0
    xor cx, cx
  .equal:
    _push 1
    ret

_print_hex: ; ( number -- )
  _pop
  mov cx, 4
  rol ax, 12     ; rotate MSB nibble into upper 4 bits
  .ph:
    mov bl, ah
    and bl, 0xf
    add bl, '0'
    cmp bl, '9'
    jbe .ok
    add bl, 7
  .ok:
    pusha
    mov al, bl
    _push ax
    call _emit
    popa
    rol ax, 4
    loop .ph
    ret


start:
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov bp, 0x9000

  _push msg
  call _print
  _push 123
  call _print_dec
  _push 10
  call _emit

  _push 0x1234
  call _print_hex
  _push 10
  call _emit

  ; _push 0x0800
  ; _push 2
  ; call _load
  ; call _goto

  hlt
  jmp $

msg: db 'meow ^^', 10, 0

times 510-($-$$) db 0
  dw 0xAA55
