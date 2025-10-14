[BITS 16]
[ORG 0x7C00]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax

    mov si, msg
.print:
    lodsb
    cmp al, 0
    je load_stage2
    mov ah, 0x0E
    mov bh, 0
    mov bl, 0x07
    int 0x10
    jmp .print

load_stage2:
    mov bx, 0x8000       ; load stage2 at 0x8000
    mov es, bx
    xor bx, bx
    mov ch, 0
    mov dh, 0
    mov cl, 1
    mov al, 20           ; number of sectors
    mov dl, 0x00         ; floppy
    call read_sectors
    jmp 0x0000:0x8000

read_sectors:
    pusha
.next:
    mov ah, 0x02
    mov al, 1
    int 0x13
    jc .error
    add bx, 512
    loop .next
    popa
    ret

.error:
    mov si, err
    mov ah, 0x0E
.errloop:
    lodsb
    or al, al
    jz $
    int 0x10
    jmp .errloop

msg db 'Booting...',0
err db 'Disk read error!',0
times 510-($-$$) db 0
dw 0xAA55
