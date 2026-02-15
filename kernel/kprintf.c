#include "kprintf.h"
#include <stdarg.h>
#include "io.h"

void print_char(char c) {
    asm volatile(
        "mov $0x0E, %%ah\n"
        "mov %0, %%al\n"
        "int $0x10"
        :
        : "r"(c)
    );
}

void kprintf(const char *s, ...) {
    va_list args;
    va_start(args, s);

    while (*s) {
        if (*s == '%' && *(s+1) == 's') {
            s+=2;
            char *str = va_arg(args, char*);
            while (*str) print_char(*str++);
            continue;
        }
        if (*s == '%' && *(s+1) == 'd') {
            s+=2;
            int n = va_arg(args, int);
            char buf[12];
            int i=0;

            if (n<0) { print_char('-'); n = -n; }

            do {
                buf[i++] = '0' + (n % 10);
                n/=10;
            } while(n);

            while(i--) print_char(buf[i]);
            continue;
        }
        print_char(*s++);
    }

    va_end(args);
}
