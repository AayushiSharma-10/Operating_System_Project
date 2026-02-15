#include "commands.h"
#include "kprintf.h"
#include "string.h"
#include "io.h"
#include "fs.h"
#include "../mm/memory.h"
#include "../process/process.h"
#include "kprintf.h"
#include "string.h"

void simple_split(const char *cmdline, char *cmd, char *arg1, char *arg2) {
    int i = 0, j = 0;

    // parse cmd
    while (cmdline[i] && cmdline[i] != ' ') {
        cmd[j++] = cmdline[i++];
    }
    cmd[j] = 0;

    // skip spaces
    while (cmdline[i] == ' ') i++;

    // parse arg1
    j = 0;
    while (cmdline[i] && cmdline[i] != ' ') {
        arg1[j++] = cmdline[i++];
    }
    arg1[j] = 0;

    while (cmdline[i] == ' ') i++;

    // parse arg2
    j = 0;
    while (cmdline[i] && cmdline[i] != '\0') {
        arg2[j++] = cmdline[i++];
    }
    arg2[j] = 0;
}


// -----------------------
// Memory commands
// -----------------------

void cmd_meminfo() {
    print_memory_map();
}

void cmd_test_memory() {
    kprintf("\n=== Testing Memory Allocation ===\n");
    
    void* ptr1 = kmalloc(100);
    kprintf("Allocated 100 bytes at %d\n", (unsigned int)ptr1);
    
    void* ptr2 = kmalloc(512);
    kprintf("Allocated 512 bytes at %d\n", (unsigned int)ptr2);
    
    void* ptr3 = kmalloc(2048);
    kprintf("Allocated 2048 bytes at %d\n", (unsigned int)ptr3);
    
    kprintf("Freeing 512 bytes at %d\n", (unsigned int)ptr2);
    kfree(ptr2);
    
    print_memory_map();
}

// -----------------------
// Process commands
// -----------------------

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

void cmd_ps() {
    print_processes();
}

void cmd_test_process() {
    kprintf("\n=== Testing Process Management ===\n");
    
    int pid1 = create_process("test_worker_1", test_process_1);
    int pid2 = create_process("test_worker_2", test_process_2);
    
    kprintf("Created processes: %d, %d\n", pid1, pid2);
    
    print_processes();
    
    // Simulate scheduling a few times
    kprintf("\n=== Simulating Scheduling ===\n");
    for (int i = 0; i < 3; i++) {
        schedule();
    }
    
    kprintf("\nTerminating process %d\n", pid1);
    terminate_process(pid1);
    
    print_processes();
}

// ---------------------------------------------------------
// MAIN COMMAND EXECUTION FUNCTION
// ---------------------------------------------------------

void commands_execute(const char *cmdline) {

    char cmd[32];
    char arg1[64];
    char arg2[128];

    arg1[0] = 0;
    arg2[0] = 0;

    simple_split(cmdline, cmd, arg1, arg2);

    // Empty command
    if (cmd[0] == 0)
        return;

    // ------------------------
    // BASIC SHELL COMMANDS
    // ------------------------
    if (strcmp(cmd,"help")==0) {
        kprintf("Commands:\n");
        kprintf("  help\n");
        kprintf("  clear\n");
        kprintf("  echo <text>\n");
        kprintf("  ls\n");
        kprintf("  create <name>\n");
        kprintf("  write <name> <text>\n");
        kprintf("  read <name>\n");
        kprintf("  del <name>\n");
        kprintf("  reboot\n");

        // NEW MEMORY + PROCESS COMMANDS
        kprintf("  meminfo\n");
        kprintf("  testmem\n");
        kprintf("  ps\n");
        kprintf("  testproc\n");

        return;
    }

    if (strcmp(cmd,"clear")==0) {
        io_clearscreen();
        return;
    }

    if (strcmp(cmd,"echo")==0) {
        kprintf("%s\n", arg1);
        return;
    }

    if (strcmp(cmd,"ls")==0) {
        list_files();
        return;
    }

    if (strcmp(cmd,"create")==0) {
        create_file(arg1);
        return;
    }

    if (strcmp(cmd,"write")==0) {
        write_file(arg1, arg2);
        return;
    }

    if (strcmp(cmd,"read")==0) {
        read_file(arg1);
        return;
    }

    if (strcmp(cmd,"del")==0) {
        delete_file(arg1);
        return;
    }

    if (strcmp(cmd,"reboot")==0) {
        asm volatile("int $0x19");
        return;
    }

    // --------------------------------------
    // MEMORY COMMANDS
    // --------------------------------------
    if (strcmp(cmd,"meminfo")==0) {
        cmd_meminfo();
        return;
    }

    if (strcmp(cmd,"testmem")==0) {
        cmd_test_memory();
        return;
    }

    // --------------------------------------
    // PROCESS COMMANDS
    // --------------------------------------
    if (strcmp(cmd,"ps")==0) {
        cmd_ps();
        return;
    }

    if (strcmp(cmd,"testproc")==0) {
        cmd_test_process();
        return;
    }

    // Unknown
    kprintf("Unknown command: %s\n", cmd);
}
