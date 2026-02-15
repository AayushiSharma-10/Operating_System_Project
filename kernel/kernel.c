#include "io.h"
#include "string.h"
#include "kprintf.h"
#include "fs.h"
#include "cli.h"
#include "commands.h"

void kmain() {
    // --- Set up a proper stack ---
    asm volatile(
        ".intel_syntax noprefix\n"
        "cli\n"
        "xor ax, ax\n"
        "mov ss, ax\n"
        "mov sp, 0x7C00\n"
        "sti\n"
        ".att_syntax prefix\n"
    );

    kprintf("=== HumeOS Kernel Loaded ===\n");
    kprintf("Type 'help' for commands.\n\n");
    kprintf("Starting CLI...\n");

    cli_run();   // start CLI shell

    for(;;);     // infinite loop (should never reach here)
}
