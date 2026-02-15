; boot/stage2.template.asm
org 0x7E00
bits 16

STAGE2_SECTORS equ 1
KERNEL_SECTORS equ __KSECS__
KERNEL_LBA     equ 1 + STAGE2_SECTORS   ; = 2

start:
    cli
    xor ax, ax
    mov ds, ax
    mov es, ax
    sti

    mov si, msg_load
    call print

    ; Destination: ES:BX = 0x1000:0x0000
    mov ax, 0x1000
    mov es, ax
    xor bx, bx

    ; prepare DAP for kernel read at label 'dap'
    lea si, [dap]                ; offset of DAP (DS is 0)
    mov byte [si + 0], 16       ; DAP size
    mov byte [si + 1], 0        ; reserved
    mov word [si + 2], KERNEL_SECTORS
    mov word [si + 4], 0x0000   ; buffer offset (0x0000)
    mov word [si + 6], 0x1000   ; buffer segment (0x1000)
    mov dword [si + 8], KERNEL_LBA
    mov dword [si + 12], 0

    ; call BIOS extended read using DL preserved from stage1
    mov ah, 0x42
    int 0x13
    jc disk_err

    mov si, msg_ok
    call print

    jmp 0x1000:0x0000

print:
    lodsb
    or al, al
    jz .ret
    mov ah, 0x0E
    int 0x10
    jmp print
.ret:
    ret

disk_err:
    mov si, msg_err
    call print
    hlt

msg_load db "Stage2: loading kernel...",0
msg_ok   db "Kernel loaded.",0
msg_err  db "Disk read error!",0

align 4
dap: times 16 db 0
times 512 - ($ - $$) db 0
