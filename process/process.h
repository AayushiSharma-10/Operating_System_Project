#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCESSES 32
#define PROCESS_NAME_LEN 16
#define PROCESS_STACK_SIZE 1024  /* 1 KB stack per process */

typedef enum {
    PROCESS_TERMINATED = 0,
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED
} ProcessState;

typedef struct {
    int pid;
    char name[PROCESS_NAME_LEN];
    ProcessState state;
    unsigned int esp;                 /* saved stack pointer (not full context) */
    unsigned int stack[PROCESS_STACK_SIZE / 4]; /* simple per-process stack */
    void (*entry_point)(void);
    int priority;
    unsigned int sleep_until;
} Process;

/* manager API */
void init_process_manager(void);
int create_process(const char *name, void (*entry)());
void terminate_process(int pid);
void schedule(void);
void yield(void);
void sleep_ticks(unsigned int ticks);
void wakeup_process(int pid);
void process_timer_tick(void);

/* info */
void print_processes(void);
Process* get_current_process(void);
int get_running_process_count(void);

#endif
