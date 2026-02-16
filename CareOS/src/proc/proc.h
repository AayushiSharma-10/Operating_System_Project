#define MAX_PROCS 10
#ifndef PROC_H
#define PROC_H

typedef enum { READY, RUNNING, WAITING, TERMINATED } proc_state;

typedef struct {
    int pid;
    char name[32];
    proc_state state;
    void (*entry_point)();
} process;

void proc_init();
int proc_create(const char* name, void (*entry)());
void proc_run();
void proc_list();
process* proc_get_all();
int proc_get_count();
int detect_deadlock();
void request_resource(int pid, int rid);
void assign_resource(int rid, int pid);
void release_resource(int rid, int pid);

#endif