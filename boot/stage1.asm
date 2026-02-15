; Stage 1 (MBR) - loads stage2 into 0x7E00 using LBA
org 0x7C00
bits 16

STAGE2_SECTORS equ 1     ; stage2.bin is 512 bytes → 1 sector

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    mov si, msg
    call print

    ; Disk Address Packet
    lea si, [dap]
    mov byte [si+0], 16
    mov byte [si+1], 0
    mov word [si+2], STAGE2_SECTORS
    mov word [si+4], 0x7E00
    mov word [si+6], 0x0000
    mov dword [si+8], 1        ; LBA = 1 (sector after MBR)
    mov dword [si+12], 0

    mov ah, 0x42
    int 0x13
    jc disk_err

    jmp 0x0000:0x7E00

print:
    lodsb
    or al, al
    jz .ret
    mov ah,0x0E
    int 0x10
    jmp print
.ret:
    ret

disk_err:
    mov si, err
    call print
    hlt

msg db "HumeOS stage1 - loading stage2...",0
err db "Disk error!",0

align 4
dap: times 16 db 0

times 510-($-$$) db 0
dw 0xAA55
