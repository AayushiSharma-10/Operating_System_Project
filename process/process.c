#include "process.h"
#include "../kernel/kprintf.h"
#include "../kernel/string.h"
#include "../mm/memory.h"

static Process processes[MAX_PROCESSES];
static int current_pid = -1;
static int next_pid = 1;
static unsigned int system_ticks = 0;

void init_process_manager() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = -1;
        processes[i].state = PROCESS_TERMINATED;
        processes[i].name[0] = '\0';
    }
    
    // Create idle process
    create_process("idle", 0);
    
    kprintf("Process Manager: Initialized (Max %d processes)\n", MAX_PROCESSES);
}

int create_process(const char* name, void (*entry)()) {
    // Find free process slot
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_TERMINATED) {
            slot = i;
            break;
        }
    }
    
    if (slot == -1) {
        kprintf("Process Manager: Cannot create process '%s' - no free slots\n", name);
        return -1;
    }
    
    // Initialize process
    Process* p = &processes[slot];
    p->pid = next_pid++;
    strcpy(p->name, name);
    p->state = PROCESS_READY;
    p->entry_point = entry;
    p->priority = 1;
    p->sleep_until = 0;
    
    // Setup stack
    p->esp = (unsigned int)&p->stack[PROCESS_STACK_SIZE - 1];
    
    // Initialize stack with fake context for when it first runs
    unsigned int* stack = (unsigned int*)p->esp;
    
    // Push fake return address (process entry point)
    *(--stack) = (unsigned int)entry;
    
    // Push fake registers (eax, ebx, ecx, edx, esi, edi, ebp)
    for (int i = 0; i < 7; i++) {
        *(--stack) = 0;
    }
    
    p->esp = (unsigned int)stack;
    
    kprintf("Process Manager: Created process '%s' (PID: %d)\n", name, p->pid);
    return p->pid;
}

void terminate_process(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = PROCESS_TERMINATED;
            kprintf("Process Manager: Terminated process '%s' (PID: %d)\n", 
                   processes[i].name, pid);
            return;
        }
    }
    kprintf("Process Manager: Process %d not found\n", pid);
}

void schedule() {
    // Simple round-robin scheduler
    int next_pid = -1;
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        int idx = (current_pid + i + 1) % MAX_PROCESSES;
        
        if (processes[idx].state == PROCESS_READY && 
            processes[idx].sleep_until <= system_ticks) {
            next_pid = idx;
            break;
        }
    }
    
    if (next_pid == -1) {
        // No ready processes, use idle
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (processes[i].state == PROCESS_READY) {
                next_pid = i;
                break;
            }
        }
    }
    
    if (next_pid != -1 && next_pid != current_pid) {
        if (current_pid != -1) {
            processes[current_pid].state = PROCESS_READY;
        }
        current_pid = next_pid;
        processes[current_pid].state = PROCESS_RUNNING;
        
        kprintf("Scheduler: Switched to process '%s' (PID: %d)\n",
               processes[current_pid].name, processes[current_pid].pid);
    }
}

void yield() {
    schedule();
}

void sleep(unsigned int ticks) {
    if (current_pid != -1) {
        processes[current_pid].sleep_until = system_ticks + ticks;
        processes[current_pid].state = PROCESS_BLOCKED;
        kprintf("Process '%s' sleeping for %d ticks\n", 
               processes[current_pid].name, ticks);
        schedule();
    }
}

void wakeup_process(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid && processes[i].state == PROCESS_BLOCKED) {
            processes[i].state = PROCESS_READY;
            processes[i].sleep_until = 0;
            kprintf("Process %d woke up\n", pid);
            return;
        }
    }
}

void print_processes() {
    kprintf("\n=== Process Table ===\n");
    kprintf("PID\tState\t\tName\n");
    kprintf("---\t-----\t\t----\n");
    
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state != PROCESS_TERMINATED) {
            const char* state_str = "Unknown";
            switch (processes[i].state) {
                case PROCESS_READY: state_str = "Ready"; break;
                case PROCESS_RUNNING: state_str = "Running"; break;
                case PROCESS_BLOCKED: state_str = "Blocked"; break;
                case PROCESS_TERMINATED: state_str = "Terminated"; break;
            }
            
            kprintf("%d\t%s\t\t%s", processes[i].pid, state_str, processes[i].name);
            
            if (processes[i].state == PROCESS_BLOCKED) {
                kprintf(" (sleep until: %d)", processes[i].sleep_until);
            }
            
            if (i == current_pid) {
                kprintf(" <-- CURRENT");
            }
            
            kprintf("\n");
        }
    }
    
    kprintf("\nTotal processes: %d\n", get_running_process_count());
}

Process* get_current_process() {
    if (current_pid != -1) {
        return &processes[current_pid];
    }
    return 0;
}

int get_running_process_count() {
    int count = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state != PROCESS_TERMINATED) {
            count++;
        }
    }
    return count;
}

// Timer tick handler (would be called by timer interrupt)
void process_timer_tick() {
    system_ticks++;
    
    // Wake up sleeping processes
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_BLOCKED && 
            processes[i].sleep_until <= system_ticks) {
            processes[i].state = PROCESS_READY;
        }
    }
}