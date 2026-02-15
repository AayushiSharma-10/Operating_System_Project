#include "cli.h"
#include "kprintf.h"
#include "string.h"
#include "commands.h"
#include "io.h"

static void show_help() {
    kprintf("\nAvailable commands:\n");
    kprintf("help - Show this help\n");
    kprintf("meminfo - Show memory info\n");
    kprintf("ps - Show process list\n");
    kprintf("testmem - Test memory allocation\n");
    kprintf("testproc - Test process management\n");
    kprintf("clear - Clear the screen\n");
}

static void clear_screen() {
    io_clearscreen();
}

void cli_run() {
    char input[128];
    kprintf("\n--- HumeOS CLI Ready ---\n");
    while (1) {
        kprintf("\n> ");
        gets(input);
        if (strcmp(input, "help") == 0) show_help();
        else if (strcmp(input, "meminfo") == 0) cmd_meminfo();
        else if (strcmp(input, "ps") == 0) cmd_ps();
        else if (strcmp(input, "testmem") == 0) cmd_test_memory();
        else if (strcmp(input, "testproc") == 0) cmd_test_process();
        else if (strcmp(input, "clear") == 0) clear_screen();
        else kprintf("Unknown command: %s\n", input);
    }
}
