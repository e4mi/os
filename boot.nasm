; Bootloader
bits 16
org 0x7c00

; setup stack
cli
xor ax, ax
mov ds, ax
mov es, ax
mov ss, ax
mov sp, 0x7c00
sti

; load from floppy
mov ax, 0x07e0
mov es, ax
mov ax, 0x0200 + ((SIZE + 511) / 512)
mov cx, 0x0002
xor dx, dx
xor bx, bx
int 0x13
jc .stop

; start kernel
jmp 0x7e00

; stop on error
.stop: hlt
jmp .stop

; boot signature
times 510-($-$$) db 0
dw 0xaa55

incbin FILE

; padding to sectors needed by blinkenlights
times (512 - ($ - $$) % 512) % 512 db 0