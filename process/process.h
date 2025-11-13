#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 8
#define PROCESS_STACK_SIZE 4096

typedef enum {
    PROCESS_CREATED,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} ProcessState;

typedef struct {
    int pid;
    char name[32];
    ProcessState state;
    unsigned int esp;        // Stack pointer
    unsigned int stack[PROCESS_STACK_SIZE];
    void (*entry_point)();   // Function to execute
    int priority;
    unsigned int sleep_until; // For sleep functionality
} Process;

// Process management functions
void init_process_manager();
int create_process(const char* name, void (*entry)());
void terminate_process(int pid);
void schedule();
void yield();
void sleep(unsigned int ticks);
void wakeup_process(int pid);
void print_processes();

// Getters
Process* get_current_process();
int get_running_process_count();

#endif