#include "mm.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

typedef struct Block {
    size_t size;
    int free;
    struct Block* next;
} Block;

#define ALIGN4(x) (((((x)-1) >> 2) << 2) + 4)

static uint8_t heap[HEAP_SIZE];
static Block* free_list = NULL;

void mm_init() {
    free_list = (Block*)heap;
    free_list->size = HEAP_SIZE - sizeof(Block);
    free_list->free = 1;
    free_list->next = NULL;
}

void* mm_alloc(size_t size) {
    size = ALIGN4(size);
    Block* curr = free_list;

    while (curr) {
        if (curr->free && curr->size >= size) {
            if (curr->size > size + sizeof(Block)) {
                Block* new_block = (Block*)((uint8_t*)curr + sizeof(Block) + size);
                new_block->size = curr->size - size - sizeof(Block);
                new_block->free = 1;
                new_block->next = curr->next;

                curr->size = size;
                curr->next = new_block;
            }
            curr->free = 0;
            return (void*)((uint8_t*)curr + sizeof(Block));
        }
        curr = curr->next;
    }

    return NULL;
}

void mm_free(void* ptr) {
    if (!ptr) return;

    Block* block = (Block*)((uint8_t*)ptr - sizeof(Block));
    if (block->free) {
        printf("Warning: Double free detected at %p\n", ptr);
        return;
    }

    block->free = 1;

    // Coalesce adjacent free blocks
    Block* curr = free_list;
    while (curr && curr->next) {
        if (curr->free && curr->next->free) {
            curr->size += sizeof(Block) + curr->next->size;
            curr->next = curr->next->next;
        } else {
            curr = curr->next;
        }
    }
}

void mm_status() {
    Block* curr = free_list;
    size_t used = 0, free = 0;
    int block_count = 0;

    printf("Heap Blocks:\n");
    while (curr) {
        printf("  Block %d: %s, Size: %zu bytes, Addr: %p\n",
               block_count++, curr->free ? "Free" : "Used", curr->size, (void*)curr);
        if (curr->free)
            free += curr->size;
        else
            used += curr->size;
        curr = curr->next;
    }

    printf("\nMemory Status:\n");
    printf("Used: %zu bytes\n", used);
    printf("Free: %zu bytes\n", free);
    printf("Total: %zu bytes\n", (size_t)HEAP_SIZE);
}

char* mm_strdup(const char* src) {
    size_t len = strlen(src) + 1;
    char* dest = (char*)mm_alloc(len);
    if (dest) strcpy(dest, src);
    return dest;
}

void mm_reset() {
    mm_init();
    printf("Heap reset.\n");
}

void mm_dump() {
    Block* curr = free_list;
    int block_count = 0;
    printf("Heap Dump:\n");
    while (curr) {
        printf("  Block %d: %s, Size: %zu bytes, Addr: %p, Next: %p\n",
               block_count++, curr->free ? "Free" : "Used", curr->size,
               (void*)curr, (void*)curr->next);
        curr = curr->next;
    }
}