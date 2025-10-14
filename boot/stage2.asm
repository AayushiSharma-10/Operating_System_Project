[BITS 16]
[ORG 0x8000]

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    ; -------------------------
    ; Print message
    ; -------------------------
    mov si, msg
.print:
    lodsb
    cmp al, 0
    je load_kernel
    mov ah, 0x0E
    int 0x10
    jmp .print

; -------------------------
; Load kernel
; -------------------------
load_kernel:
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    mov dl, 0          ; drive 0
    mov dh, 0          ; head 0
    mov ch, 0          ; track 0
    mov byte [sector_num], 1   ; starting sector
    mov cx, 20                 ; number of sectors to read

load_loop:
    mov ah, 0x02        ; BIOS read sector
    mov al, 1           ; read 1 sector
    mov cl, [sector_num]  ; 8-bit sector number
    int 0x13
    jc fail

    add bx, 512
    cmp bx, 0x0000
    jne no_seg

    mov ax, es
    add ax, 1
    mov es, ax

no_seg:
    inc byte [sector_num]
    loop load_loop

; -------------------------
; Enable protected mode
; -------------------------
cli
lgdt [gdt_desc]
mov eax, cr0
or eax, 1
mov cr0, eax
jmp 0x08:pm_entry

[BITS 32]
pm_entry:
    mov ax, 0x10
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov esp, 0x90000
    jmp 0x08:0x00100000

fail:
    mov si, err
.fail_loop:
    lodsb
    or al, al
    jz $
    mov ah, 0x0E
    int 0x10
    jmp .fail_loop

gdt:
dq 0
dq 0x00CF9A000000FFFF
dq 0x00CF92000000FFFF

gdt_desc:
dw gdt_desc - gdt - 1
dd gdt

sector_num db 0
msg db 'Stage2 running...',0
err db 'Disk read error!',0

times 510-($-$$) db 0