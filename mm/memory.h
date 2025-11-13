#ifndef MEMORY_H
#define MEMORY_H

#define MEMORY_SIZE 0x100000  // 1MB total memory
#define KERNEL_HEAP_START 0x200000  // 2MB mark for kernel heap
#define PAGE_SIZE 4096

typedef struct {
    unsigned int start;
    unsigned int size;
    int used;
} MemoryBlock;

void init_memory();
void* kmalloc(unsigned int size);
void kfree(void* ptr);
void print_memory_map();
unsigned int get_free_memory();
unsigned int get_used_memory();

#endif