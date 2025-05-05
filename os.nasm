org 0x7c00

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax

    ; load sector 2 to 0:800h
    mov bx, 0x0800
    mov ah, 2
    mov al, 1
    xor ch, ch
    xor dh, dh
    mov dl, 0
    mov cl, 2
    int 13h

    mov si, 0x0800
    mov di, 0x1000

next:
    lodsb
    or al, al
    jz run
    cmp al, ';'
    je skip_comment
    cmp al, '#'
    jne next

    lodsb
    call hex
    shl al, 4
    mov ah, al

    lodsb
    call hex
    or al, ah
    stosb

    jmp next

skip_comment:
    lodsb
    cmp al, 10
    jne skip_comment
    jmp next


run:
    call 0x0000:0x1000
    jmp $

hex:
    cmp al, '0'
    jb bad
    cmp al, '9'
    jbe .num
    cmp al, 'a'
    jb bad
    cmp al, 'f'
    jbe .low
    jmp bad

.num:
    sub al, '0'
    ret

.low:
    sub al, 'a'
    add al, 10
    ret

bad:
    xor al, al
    ret

times 510-($-$$) db 0
dw 0xaa55
