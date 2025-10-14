#ifndef DISK_H
#define DISK_H

void read_sectors(unsigned char* buffer, unsigned short sector, unsigned short count);
void write_sectors(unsigned char* buffer, unsigned short sector, unsigned short count);

#endif
