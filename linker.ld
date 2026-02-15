OUTPUT_FORMAT(elf32-i386)
ENTRY(kmain)

SECTIONS
{
    /* Kernel is loaded at physical 0x10000 by stage2 */
    . = 0x00010000;

    .text : {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : {
        *(.data*)
    }

    .bss : {
        __bss_start = .;
        *(COMMON)
        *(.bss*)
        __bss_end = .;
    }

    end = .;
}

/* optional convenience symbol for stack top */
PROVIDE(__stack_top = 0x0009F000);
