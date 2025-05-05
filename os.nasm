org 0x7c00
%define ipush(x) push x
%define ipop() pop ax

jmp start

%macro push 1
  mov ax, %1
  sub bp, 2
  mov [bp], ax
%endmacro

%macro pop 0
  mov ax, [bp]
  add bp, 2
%endmacro

load_fda: ; ( addr sector -- addr )
  pop
  mov cl, al
  pop
  mov bx, ax
  xor dh, dh
  xor ch, ch
  mov dl, 0
  mov ah, 2
  mov al, 1
  int 13h
  push bx
  ret

goto_addr: ; ( addr -- )
  pop
  jmp ax

emit: ; ( char -- )
  pop
  mov ah, 0x0e
  int 10h
  cmp al, 10
  jne .nonl
  mov al, 13
  int 10h
  .nonl:
  ret

print_str: ; ( addr -- )
  pop
  mov si, ax
.pls:
  lodsb
  or al, al
  jz .plend
  push ax
  call emit
  jmp .pls
.plend:
  ret

print_num: ; ( num -- )
  pop
  mov cx, 0
  mov bx, 10
.pn1:
  xor dx, dx
  div bx         ; AX=quot, DX=rem
  push dx
  inc cx
  or ax, ax
  jnz .pn1
.pn2:
  pop             ; DX
  add dl, '0'
  push dx
  call emit
  loop .pn2
  ret

start:
  xor ax, ax
  mov ds, ax
  mov es, ax
  mov bp, 0x9000

  push msg
  call print_str
  jmp $

  push 0x0800
  push 2
  call load_fda
  call goto_addr

  hlt
  jmp $

msg: db 'meow ^^', 10, 0

times 510-($-$$) db 0
  dw 0xAA55
