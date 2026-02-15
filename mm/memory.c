#include "memory.h"
#include "../kernel/kprintf.h"

#define MAX_MEMORY_BLOCKS 128

static MemoryBlock memory_blocks[MAX_MEMORY_BLOCKS];
static unsigned int heap_pointer = KERNEL_HEAP_START;

void init_memory() {

    // Clear block list
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        memory_blocks[i].used = 0;
        memory_blocks[i].size = 0;
        memory_blocks[i].start = 0;
    }

    // Reserve block #0 for the kernel itself (1MB–1.2MB approx)
    memory_blocks[0].start = 0x00100000;
    memory_blocks[0].size  = 0x00020000;
    memory_blocks[0].used  = 1;

    heap_pointer = KERNEL_HEAP_START;

    kprintf("Memory: initialized (heap at 0x%x)\n", heap_pointer);
}

void* kmalloc(unsigned int size) {

    if (heap_pointer + size > KERNEL_HEAP_START + HEAP_SIZE) {
        kprintf("kmalloc: Out of memory! (%d bytes)\n", size);
        return 0;
    }

    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (!memory_blocks[i].used) {

            memory_blocks[i].used  = 1;
            memory_blocks[i].start = heap_pointer;
            memory_blocks[i].size  = size;

            void* ptr = (void*)heap_pointer;

            heap_pointer += size;
            heap_pointer = (heap_pointer + 3) & ~3;   // 4-byte align

            return ptr;
        }
    }

    kprintf("kmalloc: no free memory blocks!\n");
    return 0;
}

void kfree(void* ptr) {
    unsigned int addr = (unsigned int)ptr;

    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (memory_blocks[i].used && memory_blocks[i].start == addr) {
            memory_blocks[i].used = 0;
            return;
        }
    }

    kprintf("kfree: invalid pointer 0x%x\n", addr);
}

unsigned int get_free_memory() {
    return (KERNEL_HEAP_START + HEAP_SIZE) - heap_pointer;
}

unsigned int get_used_memory() {
    unsigned int total = 0;
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++)
        if (memory_blocks[i].used)
            total += memory_blocks[i].size;
    return total;
}

void print_memory_map() {

    kprintf("\n=== Memory Map ===\n");
    kprintf("Physical memory: %d MB\n", MEMORY_SIZE / (1024*1024));

    kprintf("Kernel Heap: 0x%x - 0x%x\n",
        KERNEL_HEAP_START,
        KERNEL_HEAP_START + HEAP_SIZE
    );

    kprintf("\nBlocks allocated:\n");

    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (memory_blocks[i].used) {
            kprintf("Block %d: 0x%x - 0x%x (%d bytes)\n",
                i,
                memory_blocks[i].start,
                memory_blocks[i].start + memory_blocks[i].size,
                memory_blocks[i].size
            );
        }
    }

    kprintf("\nUsed: %d bytes, Free: %d bytes\n",
        get_used_memory(),
        get_free_memory()
    );
}
