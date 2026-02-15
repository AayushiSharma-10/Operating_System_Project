#include "process.h"
#include "../kernel/kprintf.h"
#include "../kernel/string.h"
#include "../mm/memory.h"   /* optional if processes allocate memory */

/* process table and state */
static Process processes[MAX_PROCESSES];
static int current_index = -1;    /* index in processes[] of current running process */
static int next_pid = 1;
static unsigned int system_ticks = 0;

void init_process_manager(void) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        processes[i].pid = -1;
        processes[i].state = PROCESS_TERMINATED;
        processes[i].name[0] = '\0';
        processes[i].entry_point = 0;
        processes[i].esp = 0;
        processes[i].priority = 0;
        processes[i].sleep_until = 0;
    }

    /* create an idle process (no-op) so scheduler always has something to run */
    create_process("idle", 0);

    kprintf("Process Manager: Initialized (max %d procs)\n", MAX_PROCESSES);
}

/* create a process and return pid, or -1 on failure */
int create_process(const char* name, void (*entry)()) {
    int slot = -1;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_TERMINATED) {
            slot = i;
            break;
        }
    }
    if (slot == -1) {
        kprintf("Process Manager: no free slots for %s\n", name);
        return -1;
    }

    Process *p = &processes[slot];
    p->pid = next_pid++;
    strncpy(p->name, name, PROCESS_NAME_LEN - 1);
    p->name[PROCESS_NAME_LEN - 1] = '\0';
    p->state = PROCESS_READY;
    p->entry_point = entry;
    p->priority = 1;
    p->sleep_until = 0;

    /* initialize stack pointer to top of the stack array */
    unsigned int *stack_top = &p->stack[PROCESS_STACK_SIZE/4 - 1];

    /* push fake return address so if we switch to this stack and RET we go to entry */
    if (entry) {
        *(stack_top--) = (unsigned int)entry;
    } else {
        /* if entry == NULL (idle) push a tiny loop address (NULL will fault if executed) */
        *(stack_top--) = 0;
    }

    /* push fake registers to simplify later context switch (placeholders) */
    for (int i = 0; i < 8; i++) {
        *(stack_top--) = 0;
    }

    p->esp = (unsigned int)stack_top;

    kprintf("Process Manager: Created '%s' PID=%d (slot=%d)\n", p->name, p->pid, slot);
    return p->pid;
}

void terminate_process(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = PROCESS_TERMINATED;
            kprintf("Process Manager: Terminated PID %d (%s)\n", pid, processes[i].name);
            return;
        }
    }
    kprintf("Process Manager: PID %d not found\n", pid);
}

/* very simple round-robin scheduler (cooperative unless timer interrupt calls schedule) */
void schedule(void) {
    int start = (current_index >= 0) ? current_index : 0;
    int found = -1;

    for (int i = 1; i <= MAX_PROCESSES; i++) {
        int idx = (start + i) % MAX_PROCESSES;
        if (processes[idx].state == PROCESS_READY && processes[idx].sleep_until <= system_ticks) {
            found = idx;
            break;
        }
    }

    if (found == -1) {
        /* fallback: run any ready process */
        for (int i = 0; i < MAX_PROCESSES; i++) {
            if (processes[i].state == PROCESS_READY && processes[i].sleep_until <= system_ticks) {
                found = i;
                break;
            }
        }
    }

    if (found != -1 && found != current_index) {
        if (current_index != -1 && processes[current_index].state == PROCESS_RUNNING)
            processes[current_index].state = PROCESS_READY;

        current_index = found;
        processes[current_index].state = PROCESS_RUNNING;

        kprintf("Scheduler: switched to PID %d (%s)\n",
                processes[current_index].pid, processes[current_index].name);

        /* In this minimal kernel we will directly call the function (cooperative model).
           A real kernel would perform a context switch here (save registers, load esp, iret, etc.) */
        if (processes[current_index].entry_point) {
            processes[current_index].entry_point();
            /* when the process returns, mark it terminated */
            processes[current_index].state = PROCESS_TERMINATED;
        }
    }
}

void yield(void) {
    schedule();
}

/* sleep for N ticks (cooperative) */
void sleep_ticks(unsigned int ticks) {
    if (current_index != -1) {
        processes[current_index].sleep_until = system_ticks + ticks;
        processes[current_index].state = PROCESS_BLOCKED;
        kprintf("Process %d (%s) sleeping %u ticks\n",
               processes[current_index].pid, processes[current_index].name, ticks);
        schedule();
    }
}

void wakeup_process(int pid) {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid == pid && processes[i].state == PROCESS_BLOCKED) {
            processes[i].state = PROCESS_READY;
            processes[i].sleep_until = 0;
            kprintf("Process Manager: Woke PID %d\n", pid);
            return;
        }
    }
}

/* printing helper */
void print_processes(void) {
    kprintf("\n=== Process Table ===\n");
    kprintf("PID\tState\t\tName\n");
    kprintf("---\t-----\t\t----\n");
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid != -1 && processes[i].state != PROCESS_TERMINATED) {
            const char *s = "Unknown";
            switch (processes[i].state) {
                case PROCESS_READY: s = "Ready"; break;
                case PROCESS_RUNNING: s = "Running"; break;
                case PROCESS_BLOCKED: s = "Blocked"; break;
                case PROCESS_TERMINATED: s = "Terminated"; break;
            }
            kprintf("%d\t%s\t\t%s", processes[i].pid, s, processes[i].name);
            if (processes[i].state == PROCESS_BLOCKED) {
                kprintf(" (sleep_until=%u)", processes[i].sleep_until);
            }
            if (i == current_index) kprintf(" <-- CURRENT");
            kprintf("\n");
        }
    }
    kprintf("\nTotal processes: %d\n", get_running_process_count());
}

Process* get_current_process(void) {
    if (current_index != -1) return &processes[current_index];
    return 0;
}

int get_running_process_count(void) {
    int cnt = 0;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].pid != -1 && processes[i].state != PROCESS_TERMINATED) cnt++;
    }
    return cnt;
}

/* timer tick: increases system tick count and wakes processes */
void process_timer_tick(void) {
    system_ticks++;
    for (int i = 0; i < MAX_PROCESSES; i++) {
        if (processes[i].state == PROCESS_BLOCKED && processes[i].sleep_until <= system_ticks) {
            processes[i].state = PROCESS_READY;
        }
    }
}
