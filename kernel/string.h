#ifndef STRING_H
#define STRING_H

int strcmp(const char *a, const char *b) {
    while (*a && (*a == *b)) { a++; b++; }
    return *(unsigned char*)a - *(unsigned char*)b;
}

void strcpy(char *dest, const char *src) {
    while ((*dest++ = *src++));
}

#endif
