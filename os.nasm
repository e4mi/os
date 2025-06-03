; usage: sh os.nasm
true : ; nasm os.nasm && exec qemu-system-i386 -fda os -nographic

bits 16
; setup stack
; org 0x7c00
mov sp, 0x7c00

;load sectors from floppy
; mov ax, 0x07e0
; mov es, ax
; mov ax, 0x0200 + 1 ; sectors
; mov cx, 0x0002
; xor dx, dx
; xor bx, bx
; int 0x13

main:
  push 'm'
  call emit
  push 'e'
  call emit
  push 'o'
  call emit
  push 'w'
  call emit
  push 0x0a
  call emit
  call .line
.line:
  push '>'
  call emit
  push ' '
  call emit
.in:
  call dup
  call key
  call dup
  call emit
  push 0x0a
  call eq
  call swap
  call if
  jmp .in
  call exit

dup: ; (x -- x x)
  pop bp
  pop ax
  push ax
  push ax
  jmp bp

swap: ; (x y -- y x)
  pop bp
  pop ax
  pop bx
  push ax
  push bx
  jmp bp

eq: ; (x y -- eq)
  pop bp
  pop ax
  pop bx
  cmp ax, bx
  jne .f
.t:
  push 1
  jmp bp
.f:
  push 0
  jmp bp

emit: ; (byte -- )
  pop bp
  pop ax
  mov ah, 0x0e
  int 0x10
  cmp al, 0x0a
  jne .f
  mov al, 0x0d
  int 0x10
.f:
  jmp bp

key: ; ( -- byte)
  pop bp
  mov ah, 0x00
  int 0x16
  xor ah, ah
  cmp al, 0x0d
  jne .end
  mov al, 0x0a
.end:
  push ax
  jmp bp

if: ; (addr expr -- )
  pop bp
  pop ax
  pop bx
  test bx, bx
  jz .f
.t:
  ; push bp
  jmp ax
.f:
  jmp bp

sum: ; (a b -- sum)
  pop bp
  pop ax
  pop bx
  add bx, ax
  push bx
  jmp bp

read: ; (addr -- byte)
  pop bp
  pop bx
  mov ax, [bx]
  push ax
  jmp bp

write: ; (addr byte -- )
  pop bp
  pop bx
  pop ax
  mov [bx], ax
  jmp bp

exit: ; ( -- )
  hlt
  jmp exit

; boot signature
times 510-($-$$) db 0
dw 0xaa55
; padding
times (512 - ($ - $$) % 512) % 512 db 0