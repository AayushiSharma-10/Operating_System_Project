#include "io.h"
#include "kprintf.h"
#include "string.h"

void gets(char *buf) {
    int pos = 0;
    char c;
    while (1) {
        c = io_getchar(); // You must have this implemented to get input char
        if (c == '\n' || c == '\r') {
            buf[pos] = 0;
            kprintf("\n");
            return;
        }
        if (c == 8 || c == 127) { // Backspace handling
            if (pos > 0) {
                pos--;
                kprintf("\b \b");
            }
        } else {
            buf[pos++] = c;
            kprintf("%c", c);
        }
    }
}

// Low-level BIOS keyboard read
char io_getchar() {
    char c;
    asm volatile(
        "mov $0, %%ah\n"
        "int $0x16\n"
        "mov %%al, %0"
        : "=r"(c)
        :
        : "ax"
    );
    return c;
}

// Polling PS/2 keyboard
char getc() {
    char c = 0;
    while (1) {
        unsigned char status;
        asm volatile("inb $0x64, %0" : "=a"(status));

        if (status & 1) {
            asm volatile("inb $0x60, %0" : "=a"(c));
            return c;
        }
    }
}

// Read a line from keyboard (like gets)
void io_readline(char *buf, int max) {
    int i = 0;
    while (i < max - 1) {
        char c = io_getchar();
        if (c == '\r' || c == '\n') {
            kprintf("\n");
            break;
        }
        if (c == '\b') {
            if (i > 0) {
                i--;
                kprintf("\b \b");
            }
            continue;
        }
        buf[i++] = c;
        kprintf("%c", c);
    }
    buf[i] = 0;
}

// Clears the screen using BIOS interrupt
void io_clearscreen() {
    asm volatile(
        "mov $0x06, %%ah\n"      // scroll up
        "mov $0, %%al\n"         // clear entire window
        "mov $0, %%cx\n"
        "mov $0x184f, %%dx\n"
        "mov $0x07, %%bh\n"      // normal attribute
        "int $0x10"
        :
        :
        : "ax", "bx", "cx", "dx"
    );
}
