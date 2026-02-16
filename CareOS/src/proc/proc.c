#include "proc.h"
#include <stdio.h>
#include <string.h>
#define MAX_PROCS 10
#define MAX_RESOURCES 10

int request[MAX_PROCS][MAX_RESOURCES];
int assignment[MAX_RESOURCES][MAX_PROCS];
static process procs[MAX_PROCS];
static int proc_count = 0;
static int next_pid = 1;

void proc_init() {
    proc_count = 0;
    memset(request, 0, sizeof(request));
    memset(assignment, 0, sizeof(assignment));
}

process* proc_get_all() {
    return procs;
}

int proc_get_count() {
    return proc_count;
}

void run_simulated_process(const char* name) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (strcmp(procs[i].name, name) == 0 && procs[i].state == READY) {
            procs[i].state = RUNNING;
            printf("Running process %s (PID %d)\n", procs[i].name, procs[i].pid);
            if (procs[i].entry_point)
                procs[i].entry_point();
            procs[i].state = TERMINATED;
            return;
        }
    }
    printf("No READY process named '%s' found.\n", name);
}

int proc_create(const char* name, void (*entry)()) {
    if (proc_count >= MAX_PROCS) return -1;
    process* p = &procs[proc_count++];
    p->pid = next_pid++;
    strcpy(p->name, name);
    p->state = READY;
    p->entry_point = entry; // may be NULL for real processes
    return p->pid;
}

void proc_run() {
    for (int i = 0; i < proc_count; i++) {
        if (procs[i].state == READY) {
            procs[i].state = RUNNING;
            printf("Running process %s (PID %d)\n", procs[i].name, procs[i].pid);
            if (procs[i].entry_point) {
                procs[i].entry_point();
            } else {
                printf("No entry point for process '%s'. Skipping execution.\n", procs[i].name);
            }
            procs[i].state = TERMINATED;
        }
    }
}

void proc_list() {
    printf("Process Table:\n");
    for (int i = 0; i < proc_count; i++) {
        printf("PID %d: %s [%s]\n", procs[i].pid, procs[i].name,
            procs[i].state == READY ? "READY" :
            procs[i].state == RUNNING ? "RUNNING" :
            procs[i].state == WAITING ? "WAITING" : "TERMINATED");
    }
}


void request_resource(int pid, int rid) {
    request[pid][rid] = 1;
}

void assign_resource(int rid, int pid) {
    assignment[rid][pid] = 1;
    request[pid][rid] = 0;
}

void release_resource(int rid, int pid) {
    assignment[rid][pid] = 0;
}

int visited[MAX_PROCS + MAX_RESOURCES];
int rec_stack[MAX_PROCS + MAX_RESOURCES];

int is_cyclic_util(int node, int total_nodes) {
    visited[node] = 1;
    rec_stack[node] = 1;

    for (int i = 0; i < total_nodes; i++) {
        if ((node < MAX_PROCS && request[node][i]) || 
            (node >= MAX_PROCS && assignment[node - MAX_PROCS][i])) {
            if (!visited[i] && is_cyclic_util(i, total_nodes))
                return 1;
            else if (rec_stack[i])
                return 1;
        }
    }

    rec_stack[node] = 0;
    return 0;
}

int detect_deadlock() {
    int total_nodes = MAX_PROCS + MAX_RESOURCES;
    memset(visited, 0, sizeof(visited));
    memset(rec_stack, 0, sizeof(rec_stack));

    for (int i = 0; i < total_nodes; i++) {
        if (!visited[i]) {
            if (is_cyclic_util(i, total_nodes))
                return 1;
        }
    }
    return 0;
}