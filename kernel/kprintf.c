#include "kprintf.h"
#include <stdarg.h>
#include "io.h"   // putc() defined here

void kprintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);

    for (const char *p = fmt; *p; p++) {
        if (*p == '%') {
            p++;
            if (*p == 's') {
                char *s = va_arg(args, char *);
                while (*s) putc(*s++);
            } else {
                putc(*p);
            }
        } else {
            putc(*p);
        }
    }

    va_end(args);
}
