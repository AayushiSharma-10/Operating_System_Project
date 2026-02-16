[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov si, msg
    call print_string

    ; Load shell.bin from disk to 0x1000
    mov ah, 0x02
    mov al, 20          ; Number of sectors (adjust if needed)
    mov ch, 0
    mov cl, 2           ; Start at sector 2
    mov dh, 0
    mov dl, 0x00
    mov bx, 0x1000
    int 0x13
    jc disk_error

    jmp 0x1000          ; Jump to shell logic

print_string:
    mov ah, 0x0E
.next_char:
    lodsb
    or al, al
    jz .done
    int 0x10
    jmp .next_char
.done:
    ret

msg db "Booting MiniOS...", 0
disk_error:
    mov si, err_msg
    call print_string
    hlt

err_msg db "Disk read failed!", 0

times 510-($-$$) db 0
dw 0xAA55