#include "cli.h"
#include "commands.h"
#include "../kernel/kprintf.h"  // Changed this line
#include "../kernel/string.h"   // Changed this line

#define MAX_COMMAND_LENGTH 64
#define MAX_ARGUMENTS 8

void print_banner() {
    kprintf("\n");
    kprintf("===========================================\n");
    kprintf("          Hume OS Command Line\n");
    kprintf("===========================================\n");
    kprintf("Type 'help' for available commands\n");
    kprintf("\n");
}

void print_help() {
    kprintf("Available commands:\n");
    kprintf("  help          - Show this help message\n");
    kprintf("  clear         - Clear the screen\n");
    kprintf("  echo <text>   - Print text to screen\n");
    kprintf("  create <file> - Create a new file\n");
    kprintf("  write <file> <text> - Write text to file\n");
    kprintf("  read <file>   - Read file contents\n");
    kprintf("  delete <file> - Delete a file\n");
    kprintf("  list          - List all files\n");
    kprintf("  time          - Show system time (dummy)\n");
    kprintf("  reboot        - Reboot system (dummy)\n");
    kprintf("  exit          - Exit CLI (will restart)\n");
}

void clear_screen() {
    // Simple clear screen by printing many newlines
    for (int i = 0; i < 50; i++) {
        kprintf("\n");
    }
}

void echo_command(const char* text) {
    if (text[0] == '\0') {
        kprintf("Usage: echo <text>\n");
        return;
    }
    kprintf("%s\n", text);
}

void create_file_command(const char* filename) {
    if (filename[0] == '\0') {
        kprintf("Usage: create <filename>\n");
        return;
    }
    create_file(filename);
}

void write_file_command(const char* args) {
    char filename[32] = {0};
    char text[256] = {0};
    
    // Simple parsing: first word is filename, rest is text
    int i = 0;
    while (args[i] != ' ' && args[i] != '\0') {
        filename[i] = args[i];
        i++;
    }
    
    if (args[i] == ' ') {
        i++; // skip space
        int j = 0;
        while (args[i] != '\0' && j < 255) {
            text[j++] = args[i++];
        }
        text[j] = '\0';
    }
    
    if (filename[0] == '\0' || text[0] == '\0') {
        kprintf("Usage: write <filename> <text>\n");
        return;
    }
    
    write_file(filename, text);
}

void read_file_command(const char* filename) {
    if (filename[0] == '\0') {
        kprintf("Usage: read <filename>\n");
        return;
    }
    read_file(filename);
}

void delete_file_command(const char* filename) {
    if (filename[0] == '\0') {
        kprintf("Usage: delete <filename>\n");
        return;
    }
    delete_file(filename);
}

void execute_command(const char* command) {
    char cmd[32] = {0};
    const char* args = command;
    
    // Extract command name (first word)
    int i = 0;
    while (command[i] != ' ' && command[i] != '\0') {
        cmd[i] = command[i];
        i++;
    }
    
    // Skip to arguments
    while (command[i] == ' ') i++;
    args = &command[i];
    
    // Execute command
    if (strcmp(cmd, "help") == 0) {
        print_help();
    }
    else if (strcmp(cmd, "clear") == 0) {
        clear_screen();
    }
    else if (strcmp(cmd, "echo") == 0) {
        echo_command(args);
    }
    else if (strcmp(cmd, "create") == 0) {
        create_file_command(args);
    }
    else if (strcmp(cmd, "write") == 0) {
        write_file_command(args);
    }
    else if (strcmp(cmd, "read") == 0) {
        read_file_command(args);
    }
    else if (strcmp(cmd, "delete") == 0) {
        delete_file_command(args);
    }
    else if (strcmp(cmd, "list") == 0) {
        list_files();
    }
    else if (strcmp(cmd, "time") == 0) {
        kprintf("System time: Not implemented yet\n");
    }
    else if (strcmp(cmd, "reboot") == 0) {
        kprintf("System reboot: Not implemented yet\n");
    }
    else if (strcmp(cmd, "exit") == 0) {
        kprintf("Exiting CLI...\n");
    }
    else if (cmd[0] != '\0') {
        kprintf("Unknown command: '%s'. Type 'help' for available commands.\n", cmd);
    }
    // Add to the execute_command function in cli.c
else if (strcmp(command, "meminfo") == 0) {
    cmd_meminfo();
}
else if (strcmp(command, "ps") == 0) {
    cmd_ps();
}
else if (strcmp(command, "testmem") == 0) {
    cmd_test_memory();
}
else if (strcmp(command, "testproc") == 0) {
    cmd_test_process();
}
}

void run_cli() {
    // This would normally read from keyboard input
    // For now, we'll simulate some commands
    
    kprintf("Starting Command Line Interface...\n");
    
    // Simulate user typing commands
    kprintf("\n$ ");
    execute_command("help");
    
    kprintf("\n$ ");
    execute_command("create test.txt");
    
    kprintf("\n$ ");
    execute_command("write test.txt Hello from CLI!");
    
    kprintf("\n$ ");
    execute_command("read test.txt");
    
    kprintf("\n$ ");
    execute_command("list");
    
    kprintf("\n$ ");
    execute_command("echo This is a test message");
    
    kprintf("\n$ ");
    execute_command("delete test.txt");
    
    kprintf("\n$ ");
    execute_command("list");
    
    kprintf("\n$ ");
    execute_command("unknown_command");
    
    kprintf("\nCLI session completed. System ready.\n");
}

void init_cli() {
    // Initialize command line interface
    kprintf("Command Line Interface initialized\n");
}