#ifndef DISK_H
#define DISK_H
#include <stdint.h>
void disk_read_sectors(uint32_t lba, uint32_t count, void* buf);
#endif