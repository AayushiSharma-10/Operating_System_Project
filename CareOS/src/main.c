#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "fs/fs.h"
#include "shell/shell.h"
#include "proc/proc.h"
#include "mm/mm.h"

#ifdef HAVE_LIBNOTIFY
#include <libnotify/notify.h>
#endif

void shell_task() {
    shell();
}

static void show_popup(const char *title, const char *message) {
    if (!title) title = "MiniOS";
    if (!message) message = "";

    char safe_title[256];
    char safe_msg[1024];
    strncpy(safe_title, title, sizeof(safe_title)-1);
    safe_title[sizeof(safe_title)-1] = '\0';
    strncpy(safe_msg, message, sizeof(safe_msg)-1);
    safe_msg[sizeof(safe_msg)-1] = '\0';

#ifdef HAVE_LIBNOTIFY
    if (notify_init("MiniOS")) {
        NotifyNotification *n = notify_notification_new(safe_title, safe_msg, NULL);
        if (n) {
            notify_notification_show(n, NULL);
            g_object_unref(G_OBJECT(n));
        }
    }
#else
    pid_t pid = fork();
    if (pid < 0) return;
    if (pid == 0) {
        execlp("notify-send", "notify-send", safe_title, safe_msg, (char *)NULL);
        execlp("zenity", "zenity", "--info", "--title", safe_title, "--text", safe_msg, (char *)NULL);
        _exit(0);
    }
    (void)waitpid(pid, NULL, WNOHANG);
#endif
}

/*
 * Launch a real terminal window that runs Ubuntu-style commands.
 * Uses gnome-terminal or xterm to execute a scripted shell session.
 */
static void launch_ubuntu_terminal() {
    pid_t pid = fork();
    if (pid < 0) return;

    if (pid == 0) {
        const char *cmd =
            "echo 'Welcome to Ubuntu 22.04 LTS'; "
            "uname -a; "
            "ls; "
            "/home/naveen_singh_rawat/Project/MiniOS/shell; "
            "echo 'MiniOS shell exited.'; "
            "exec bash";

        execlp("gnome-terminal", "gnome-terminal",
               "--title=Ubuntu Shell",
               "--", "bash", "-c", cmd,
               (char *)NULL);

        execlp("xterm", "xterm",
               "-T", "Ubuntu Shell",
               "-e", "bash -c 'echo Welcome to Ubuntu 22.04 LTS; uname -a; ls; ./shell; echo MiniOS shell exited.; exec bash'",
               (char *)NULL);

        _exit(0);
    }

    (void)waitpid(pid, NULL, WNOHANG);
}

int main() {
    printf("Welcome to MiniOS Shell! Type 'help' for available commands\n");

    signal(SIGCHLD, SIG_IGN); // Prevent zombie processes

    show_popup("MiniOS", "Welcome to MiniOS Shell!");
    launch_ubuntu_terminal();  // ✅ Launch your shell inside Ubuntu terminal
    
    if (chdir("/home/naveen_singh_rawat/Project/MiniOS") != 0) {
        perror("chdir failed");
        return 1;
    }

    
    // Initialize subsystems
    mm_init();       // Memory manager
    proc_init();     // Process manager
    fs_init();       // File system

    // Create shell as a process
    proc_create("shell", shell_task);

    // Run all processes
    proc_run();

    printf("System Halted.\n");
    show_popup("MiniOS", "System Halted.");
    return 0;
}