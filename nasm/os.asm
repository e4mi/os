%include "lib.asm"

cpu 8086
org 0x7c00
runtime

fn main
  lit "meow ^.^", 0
  call prn
  call exit
endfn

fn emit ; (char -- )
  pop al
  mov ah, 0x0e
  int 0x10
  if
    cmp al, '\n'
  then
    mov al, '\r'
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
    cmp al, '\r'
  then
    mov al, '\n'
  endif
  push al
  ret
endfn

fn prn ; (string -- )
  pop si
  mov ah, 0x0e
  while
    read_byte al, si
    test al
  do
    push al
    call emit
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
  push2 cx, dx
  push2 ax, bx
  call add32
  ; TODO: check for overflow
  pop2 [malloc_here], [malloc_here + 2]
  ret
endfn

fn free ; (ptr:u32 -- )
  ; TODO
  ret
endfn