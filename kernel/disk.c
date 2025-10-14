#include "disk.h"

void read_sectors(unsigned char* buffer, unsigned short sector, unsigned short count) {
    unsigned short i;
    for (i = 0; i < count; i++) {
        unsigned short s = sector + i + 1;
        __asm__ __volatile__ (
            "int $0x13"
            :
            : "a"(0x0201),      // AH=2 (read), AL=1 (sector)
              "b"(buffer),
              "c"(s),           // sector number (starts at 1)
              "d"(0x0000)       // head=0, drive=0
        );
        buffer += 512;
    }
}
