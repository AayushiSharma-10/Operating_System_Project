// Add to existing commands.c file
#include "../mm/memory.h"
#include "../process/process.h"

void cmd_meminfo() {
    print_memory_map();
}

void cmd_ps() {
    print_processes();
}

void cmd_test_memory() {
    kprintf("\n=== Testing Memory Allocation ===\n");
    
    void* ptr1 = kmalloc(100);
    kprintf("Allocated 100 bytes at 0x%x\n", ptr1);
    
    void* ptr2 = kmalloc(512);
    kprintf("Allocated 512 bytes at 0x%x\n", ptr2);
    
    void* ptr3 = kmalloc(2048);
    kprintf("Allocated 2048 bytes at 0x%x\n", ptr3);
    
    kprintf("Freeing 512 bytes at 0x%x\n", ptr2);
    kfree(ptr2);
    
    print_memory_map();
}

// Test process functions
void test_process_1() {
    kprintf("Process 1: Started\n");
    kprintf("Process 1: Doing some work...\n");
    kprintf("Process 1: Finished\n");
}

void test_process_2() {
    kprintf("Process 2: Started\n");
    kprintf("Process 2: Working...\n");
    kprintf("Process 2: Done\n");
}

void cmd_test_process() {
    kprintf("\n=== Testing Process Management ===\n");
    
    int pid1 = create_process("test_worker_1", test_process_1);
    int pid2 = create_process("test_worker_2", test_process_2);
    
    kprintf("Created processes: %d, %d\n", pid1, pid2);
    
    print_processes();
    
    // Simulate some scheduling
    kprintf("\n=== Simulating Scheduling ===\n");
    for (int i = 0; i < 3; i++) {
        schedule();
    }
    
    kprintf("\nTerminating process %d\n", pid1);
    terminate_process(pid1);
    
    print_processes();
}