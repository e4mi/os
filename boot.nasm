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
mov ax, 0x0200 + ((KERNEL_SIZE + 511) / 512)
mov cx, 0x0002
xor dx, dx
xor bx, bx
int 0x13
jc .hlt

jmp 0x7e00

.hlt: hlt
jmp .hlt

; boot signature
times 510-($-$$) db 0
dw 0xaa55

incbin KERNEL_FILE

; padding to sectors needed by blinkenlights
times (512 - ($ - $$) % 512) % 512 db 0