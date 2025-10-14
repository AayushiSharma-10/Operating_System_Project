.intel_syntax noprefix
.global _start
.extern kmain
.section .text
.code32
_start:
    cli
    mov esp, 0x90000
    call kmain
.hang:
    hlt
    jmp .hang
