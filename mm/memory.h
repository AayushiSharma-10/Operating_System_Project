#ifndef MEMORY_H
#define MEMORY_H

// Real memory available to tiny OS (QEMU guarantees 16MB)
#define MEMORY_SIZE 0x01000000    // 16MB

// Kernel is at 1MB → heap starts at 1.2MB
#define KERNEL_HEAP_START 0x00120000  
#define HEAP_SIZE 0x000F0000      // ~960 KB heap

typedef struct {
    unsigned int start;
    unsigned int size;
    int used;
} MemoryBlock;

void init_memory();
void* kmalloc(unsigned int size);
void kfree(void* ptr);
void print_memory_map(void);
unsigned int get_free_memory();
unsigned int get_used_memory();

#endif
