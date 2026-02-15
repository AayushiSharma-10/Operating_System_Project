#include "string.h"

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) {
        a++; b++;
    }
    return *(unsigned char*)a - *(unsigned char*)b;
}

void strcpy(char *dest, const char *src) {
    while ((*dest++ = *src++));
}

void strncpy(char *dest, const char *src, int n) {
    for (int i = 0; i < n; i++) {
        if (src[i])
            dest[i] = src[i];
        else
            dest[i] = 0;
    }
}

int strlen(const char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}
