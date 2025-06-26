%include "lib.asm"

org 0x7c00
cpu 8086
runtime

fn main
  op prn
    lit "meow ^.^", 0
  endop
  do exit
endfn

fn emit ; (char -- )
  pop ax
  mov ah, 0x0e
  int 0x10
  if
    cmp al, 0x0a
  then
    mov al, 0x0d
    int 0x10
  endif
endfn

fn key ; (-- char)
.read:
  mov ah, 0x00
  int 0x16
  if
    cmp al, 0
  then
    jmp .read
  endif
  if
    cmp al, 0x0d
  then
    mov al, 0x0a
  endif
  mov ah, 0
  push ax
  ret
endfn

fn prn ; (string -- )
  pop si
  mov ah, 0x0e
  while
    read_byte al, si
    test al
  loop
    op emit
      push ax
    endop
  endwhile
endfn

fn clr ; ( -- )
  mov ax, 0x0003
  int 0x10
endfn

fn exit ; ( -- )
  halt
endfn

fn add32 ; (a:u32 b:u32 -- c:u32)
  pop2 ax, bx
  pop2 cx, dx
  mov di, ax
  add di, cx
  mov si, bx
  adc si, dx
  push2 di, si
endfn

malloc_here dw 0x0000, 0x0001
; malloc_max dw 0xFBFF, 0x0009

fn malloc ; (size:u32 -- ptr:u32)
  pop2 ax, bx
  mov cx, [malloc_here]
  mov dx, [malloc_here + 2]
  push2 cx, dx
  do add32, cx, dx, ax, bx
  ; TODO: check for overflow
  pop2 cx, dx
  mov [malloc_here], cx
  mov [malloc_here + 2], dx
  ret
endfn

fn free ; (ptr:u32 -- )
  ; TODO
  ret
endfn