[BITS 16]
[ORG 0x7E00]

start:
    mov si, msg
    call print_string
    hlt

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

msg db "MiniOS Kernel Loaded!", 0