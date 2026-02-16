#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "fs/fs.h"
#include "history.h"
#include "utils/utils.h"
#include "mm/mm.h"
#include "proc/proc.h"

void (*get_task(const char* name))();

void task_hello() {
    printf("Hello from MiniOS!\n");
}

void task_ping() {
    printf("Pinging localhost...\n");
    system("ping -c 3 127.0.0.1");
}

void (*get_task(const char* name))() {
    if (strcmp(name, "hello") == 0) return task_hello;
    if (strcmp(name, "ping") == 0) return task_ping;
    return NULL;
}

void shell() {
    char input[100], arg1[100], arg2[200];

    while (1) {
        printf("\nMiniOS> ");
        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = 0;

        apply_alias(input);           // Modular aliasing
        save_history(input);         // Track command history

        if (!strcmp(input, "help")) {
            printf(
                "Commands:\n"
                "General:\n"
                "  help                 - Show this help message\n"
                "  clear                - Clear the screen\n"
                "  exit                 - Exit MiniOS\n"
                "  date                 - Show current time\n"
                "  version              - Show system version\n"
                "\nFile Operations:\n"
                "  touch                - Create an empty file\n"
                "  cat                  - Display file contents\n"
                "  copy                 - Copy a file\n"
                "  move                 - Move or rename a file\n"
                "  delete               - Delete a file\n"
                "  rename               - Rename a file\n"
                "  write                - Write content to a file\n"
                "  read                 - Read a file\n"
                "  edit                 - Edit a file\n"
                "  info                 - Show file metadata\n"
                "  search               - Search keyword in files\n"
                "\nDirectory Operations:\n"
                "  mkdir/md             - Create a directory\n"
                "  rmdir/rm -r          - Remove a directory\n"
                "  cd                   - Change directory\n"
                "  ls                   - List directory contents\n"
                "\nMemory & Process:\n"
                "  mem                  - Show memory status\n"
                "  ps                   - List running processes\n"
                "  spawn                - Create a simulated process (name only)\n"
                "  run <cmd>            - Run a real system command (e.g., run ls -l)\n"
                "\nSystem & History:\n"
                "  history              - Show command history\n"
                "  export               - Files are auto-saved\n"
                "  reset                - Clear all files\n"
                "\nDeadlock & Resources:\n"
                "  req <pid> <rid>      - Process requests a resource\n"
                "  assign <rid> <pid>   - Assign resource to process\n"
                "  release <rid> <pid>  - Release resource from process\n"
                "  deadlock             - Check for deadlock in resource graph\n"
            );
        }    
        else if (!strcmp(input, "ls")) {
            fs_list();
        } else if (!strcmp(input, "history")) {
            show_history();
        } else if (!strcmp(input, "clear")) {
            clear_screen();
        } else if (!strcmp(input, "exit")) {
            break;
        } else if (!strcmp(input, "mem")) {
            mm_status();
        } else if (!strcmp(input, "ps")) {
            proc_list();
        }else if (!strcmp(input, "write")) {
            printf("Filename: ");
            fgets(arg1, sizeof(arg1), stdin);
            arg1[strcspn(arg1, "\n")] = 0;
            if (strlen(arg1) == 0) {
                printf("Error: Filename cannot be empty.\n");
                continue;
            }
            if (fs_find(arg1) >= 0) {
                printf("File already exists.\n");
                continue;
            }
            printf("Content: ");
            fgets(arg2, sizeof(arg2), stdin);
            arg2[strcspn(arg2, "\n")] = 0;
            fs_write(arg1, arg2);
        } else if (!strcmp(input, "read")) {
            printf("Filename: ");
            fgets(arg1, sizeof(arg1), stdin);
            arg1[strcspn(arg1, "\n")] = 0;

            fs_read(arg1);
        } else if (!strcmp(input, "delete")) {
            printf("Filename: ");
            fgets(arg1, sizeof(arg1), stdin);
            arg1[strcspn(arg1, "\n")] = 0;

            fs_delete(arg1);
        } else if (!strcmp(input, "edit")) {
            printf("Filename: ");
            fgets(arg1, sizeof(arg1), stdin);
            arg1[strcspn(arg1, "\n")] = 0;

            fs_edit(arg1);
        } else if (!strcmp(input, "date")) {
            time_t now = time(NULL) + (5.5 * 3600);
            struct tm *t = gmtime(&now);
            char buffer[100];
            strftime(buffer, sizeof(buffer), "%A, %d %B %Y, %I:%M:%S %p", t);
            printf("Current date and time: %s\n", buffer);
        } else if (!strcmp(input, "version")) 
            printf("MiniOS Version 1.0.0\n");
        else if (!strcmp(input, "search")) {
            printf("Keyword: ");
            fgets(arg1, sizeof(arg1), stdin);
            arg1[strcspn(arg1, "\n")] = 0;

            fs_search(arg1);
        } else if (!strcmp(input, "rename")) {
            printf("Old Filename: ");
            fgets(arg1, sizeof(arg1), stdin);
            arg1[strcspn(arg1, "\n")] = 0;

            printf("New Filename: ");
            fgets(arg2, sizeof(arg2), stdin);
            arg2[strcspn(arg2, "\n")] = 0;

            fs_rename(arg1, arg2);
        } else if (!strcmp(input, "info")) {
            printf("Filename: ");
            fgets(arg1, sizeof(arg1), stdin);
            arg1[strcspn(arg1, "\n")] = 0;

            fs_info(arg1);
        } else if (!strcmp(input, "export")) {
            printf("Files are already saved to disk automatically.\n");
        } else if (!strcmp(input, "reset")) {
            printf("If you do this all files will deleted permanently.  ");
            //fs_clear_all();
        } else if (sscanf(input, "touch %s", arg1) == 1)
            fs_touch(arg1);
        else if (strcmp(input, "touch") == 0)
            printf("Usage: touch <filename>\n");

        else if (sscanf(input, "cat %s", arg1) == 1 || sscanf(input, "type %s", arg1) == 1)
            fs_cat(arg1);
        else if (strcmp(input, "cat") == 0 || strcmp(input, "type") == 0)
            printf("Usage: cat <filename>\n");

        else if (sscanf(input, "copy %s %s", arg1, arg2) == 2 || sscanf(input, "cp %s %s", arg1, arg2) == 2)
            fs_copy(arg1, arg2);
        else if (strcmp(input, "copy") == 0 || strcmp(input, "cp") == 0)
            printf("Usage: copy <source> <destination>\n");

        else if (sscanf(input, "move %s %s", arg1, arg2) == 2 || sscanf(input, "mv %s %s", arg1, arg2) == 2)
            fs_move(arg1, arg2);
        else if (strcmp(input, "move") == 0 || strcmp(input, "mv") == 0)
            printf("Usage: move <source> <destination>\n");

        else if (sscanf(input, "mkdir %s", arg1) == 1 || sscanf(input, "md %s", arg1) == 1)
            fs_mkdir(arg1);
        else if (strcmp(input, "mkdir") == 0 || strcmp(input, "md") == 0)
            printf("Usage: mkdir <directory>\n");

        else if (sscanf(input, "rmdir %s", arg1) == 1 || sscanf(input, "rm -r %s", arg1) == 1)
            fs_rmdir(arg1);
        else if (strcmp(input, "rmdir") == 0 || strcmp(input, "rm -r") == 0)
            printf("Usage: rmdir <directory>\n");
        else if (sscanf(input, "cd %s", arg1) == 1)
            fs_cd(arg1);
        else if (strcmp(input, "cd") == 0)
            printf("Usage: cd <directory>\n");
        else if (strncmp(input, "run ", 4) == 0) {
            char *cmd = input + 4;
            pid_t pid = fork();
            if (pid == 0) {
                execl("/bin/sh", "sh", "-c", cmd, NULL);
                perror("exec failed");
                exit(1);
            } else if (pid > 0) {
            printf("Started process: %s with PID %d\n", cmd, pid);
            proc_create(cmd, NULL);
            int status;
            waitpid(pid, &status, 0);  // Wait for child to finish
        } else
            perror("fork failed");
        } else if (sscanf(input, "spawn %s", arg1) == 1) {
            int background = 0;
            size_t len = strlen(arg1);
            if (len > 0 && arg1[len - 1] == '&') {
                background = 1;
                arg1[len - 1] = '\0'; // remove '&'
                while (len > 1 && arg1[len - 2] == ' ') {
                    arg1[len - 2] = '\0'; // trim trailing space
                    len--;
                }
            }

            void (*entry)() = get_task(arg1);
            int pid = proc_create(arg1, entry);
            if (pid >= 0) {
                printf("Spawned process '%s' with PID %d%s\n", arg1, pid, background ? " [background]" : "");
                if (background && entry) {
                    // Run immediately in background
                    if (fork() == 0) {
                        entry();
                        exit(0);
                    }
                }
            } else
                printf("Error: Could not spawn process. Table may be full.\n");
        } else if (!strcmp(input, "deadlock")) {
            if (detect_deadlock())
                printf("Deadlock detected!\n");
            else
                printf("No deadlock.\n");
        } else if (strncmp(input, "req ", 4) == 0) {
            int pid, rid;
            if (sscanf(input + 4, "%d %d", &pid, &rid) == 2) {
                request_resource(pid, rid);
                char msg[50];
                sprintf(msg, "P%d requested R%d\n", pid, rid);
                printf("%s", msg);
            } else
            printf("Usage: req <pid> <rid>\n");
        }else if (strncmp(input, "assign ", 7) == 0) {
            int rid, pid;
            if (sscanf(input + 7, "%d %d", &rid, &pid) == 2) {
                assign_resource(rid, pid);
                char msg[50];
                sprintf(msg, "R%d assigned to P%d\n", rid, pid);
                printf("%s", msg);
            } else 
                printf("Usage: assign <rid> <pid>\n");
        } else if (strncmp(input, "release ", 8) == 0) {
            int rid, pid;
            if (sscanf(input + 8, "%d %d", &rid, &pid) == 2) {
                release_resource(rid, pid);
                char msg[50];
                sprintf(msg, "R%d released from P%d\n", rid, pid);
                printf("%s", msg);
            } else
                printf("Usage: release <rid> <pid>\n");
        } else
            printf("Unknown command.\n");
    }
}