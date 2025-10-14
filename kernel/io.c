#include "io.h"

#define VIDEO_MEMORY 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t *vga_buffer = (uint16_t *)VIDEO_MEMORY;
static int cursor_row = 0;
static int cursor_col = 0;

// --- Low-level I/O port output ---
void outb(uint16_t port, uint8_t val) {
    __asm__ volatile ("outb %0, %1" : : "a"(val), "Nd"(port));
}

// --- Move the cursor on screen ---
static void move_cursor() {
    uint16_t pos = cursor_row * VGA_WIDTH + cursor_col;
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(pos & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

// --- Print a single character to screen ---
void putc(char c) {
    if (c == '\n') {
        cursor_row++;
        cursor_col = 0;
    } else {
        vga_buffer[cursor_row * VGA_WIDTH + cursor_col] = (0x07 << 8) | c;
        cursor_col++;
        if (cursor_col >= VGA_WIDTH) {
            cursor_col = 0;
            cursor_row++;
        }
    }

    if (cursor_row >= VGA_HEIGHT) {
        cursor_row = 0;  // simple reset (no scrolling)
    }

    move_cursor();
}
