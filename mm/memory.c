#include "memory.h"
#include "../kernel/kprintf.h"
#include "../kernel/string.h"

#define MAX_MEMORY_BLOCKS 128
#define HEAP_SIZE 0x20000  // 128KB heap

static MemoryBlock memory_blocks[MAX_MEMORY_BLOCKS];
static unsigned int heap_start = KERNEL_HEAP_START;
static unsigned int heap_pointer = KERNEL_HEAP_START;

void init_memory() {
    // Initialize memory blocks
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        memory_blocks[i].used = 0;
        memory_blocks[i].size = 0;
        memory_blocks[i].start = 0;
    }
    
    // Reserve first block for kernel
    memory_blocks[0].start = 0x100000;  // Kernel starts at 1MB
    memory_blocks[0].size = 0x10000;    // 64KB for kernel
    memory_blocks[0].used = 1;
    
    heap_pointer = KERNEL_HEAP_START;
    kprintf("Memory Manager: Initialized (Heap at 0x%x)\n", heap_start);
}

void* kmalloc(unsigned int size) {
    // Simple bump allocator
    if (heap_pointer + size > heap_start + HEAP_SIZE) {
        kprintf("Memory Manager: Out of memory! Requested: %d bytes\n", size);
        return 0;
    }
    
    // Find free memory block
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (!memory_blocks[i].used) {
            memory_blocks[i].start = heap_pointer;
            memory_blocks[i].size = size;
            memory_blocks[i].used = 1;
            
            void* ptr = (void*)heap_pointer;
            heap_pointer += size;
            
            // Align to 4 bytes
            heap_pointer = (heap_pointer + 3) & ~3;
            
            return ptr;
        }
    }
    
    kprintf("Memory Manager: No free memory blocks\n");
    return 0;
}

void kfree(void* ptr) {
    if (!ptr) return;
    
    unsigned int addr = (unsigned int)ptr;
    
    // Find and free the memory block
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (memory_blocks[i].used && memory_blocks[i].start == addr) {
            memory_blocks[i].used = 0;
            // Note: Simple allocator doesn't reuse memory
            return;
        }
    }
    
    kprintf("Memory Manager: Attempt to free invalid pointer 0x%x\n", addr);
}

void print_memory_map() {
    kprintf("\n=== Memory Map ===\n");
    kprintf("Total Memory: %d KB\n", MEMORY_SIZE / 1024);
    kprintf("Kernel Heap: 0x%x - 0x%x (%d KB)\n", 
           heap_start, heap_start + HEAP_SIZE, HEAP_SIZE / 1024);
    
    kprintf("\nAllocated Blocks:\n");
    int used_blocks = 0;
    unsigned int total_used = 0;
    
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (memory_blocks[i].used) {
            kprintf("  Block %d: 0x%x - 0x%x (%d bytes)\n",
                   i, memory_blocks[i].start, 
                   memory_blocks[i].start + memory_blocks[i].size,
                   memory_blocks[i].size);
            used_blocks++;
            total_used += memory_blocks[i].size;
        }
    }
    
    kprintf("\nSummary: %d blocks used, %d bytes allocated\n", 
           used_blocks, total_used);
    kprintf("Free memory: %d bytes\n", get_free_memory());
}

unsigned int get_free_memory() {
    return (heap_start + HEAP_SIZE) - heap_pointer;
}

unsigned int get_used_memory() {
    unsigned int total = 0;
    for (int i = 0; i < MAX_MEMORY_BLOCKS; i++) {
        if (memory_blocks[i].used) {
            total += memory_blocks[i].size;
        }
    }
    return total;
}