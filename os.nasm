
; --- BOOTLOADER ---

org 0x7c00

boot:
  xor ax, ax
  mov ss, ax
  mov sp, 0x7c00

  .load_floppy:
    mov ah, 0x02 ; read
    mov al, 4 ; n sectors
    mov ch, 0 ; cylinder
    mov dh, 0 ; head
    mov dl, 0 ; floppy
    mov cl, 2 ; sector
    mov bx, 0x07e0 ; destination
    mov es, bx
    xor bx, bx ; offset in es
    int 0x13

  .run_kernel:
    jmp kernel

  .signature:
    times 512 - ($ - $$) - 2 db 0
    dw 0xAA55


; --- KERNEL ---
hp dw 0x7e00 + (4 * 512)

kernel:
  mov si, meow
  call print
  call lang
  jmp poweroff

meow:
  db "meow ^^", 13, 10, 0

print:
  mov ah, 0x0e
  .loop:
    lodsb
    or al, al
    jz .end
    int 0x10
    jmp .loop
  .end:
    ret

poweroff:
  mov ah, 0x19
  int 0x15
  jmp $

lang:
  mov ax, 1024
  call alloc
  push ax
  mov di, ax
  mov si, code
  call parse
  pop ax
  call ax
  ret

parse:
  xor bx, bx
  xor cx, cx
.next:
  lodsb
  or al, al
  jz .done
  cmp al, ' '
  je .next
  cmp al, '9'
  jbe .digit
  sub al, 'a' - 10
  jmp .hex
.digit:
  sub al, '0'
.hex:
  shl bx, 4
  or bl, al
  inc cx
  cmp cx, 2
  jne .next
  mov [di], bl
  inc di
  xor bx, bx
  xor cx, cx
  jmp .next
.done:
  ret

alloc:
  mov bx, [hp]
  add word [hp], ax
  mov ax, bx
  ret

code db 'b4 0e b0 6f cd 10 b0 6b cd 10 c3', 0
align 16

times 4 * 512 - ($ - $$) db 0