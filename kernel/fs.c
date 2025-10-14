#include "fs.h"
#include "kprintf.h"
#include "string.h"

#define MAX_FILES 16
#define MAX_FILENAME 32
#define MAX_FILE_SIZE 512

typedef struct {
    char name[MAX_FILENAME];
    char data[MAX_FILE_SIZE];
    int size;
    int in_use;
} File;

File file_table[MAX_FILES];

// Create a file
void create_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use && strcmp(file_table[i].name, name) == 0) {
            kprintf("Error: File '%s' already exists\n", name);
            return;
        }
    }
    for (int i = 0; i < MAX_FILES; i++) {
        if (!file_table[i].in_use) {
            strcpy(file_table[i].name, name);
            file_table[i].size = 0;
            file_table[i].in_use = 1;
            kprintf("File '%s' created\n", name);
            return;
        }
    }
    kprintf("Error: File table full\n");
}

// Write data to a file
void write_file(const char *name, const char *data) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use && strcmp(file_table[i].name, name) == 0) {
            int len = 0;
            while (data[len] != '\0' && len < MAX_FILE_SIZE - 1) {
                file_table[i].data[len] = data[len];
                len++;
            }
            file_table[i].data[len] = '\0';
            file_table[i].size = len;
            kprintf("Data written to '%s'\n", name);
            return;
        }
    }
    kprintf("Error: File '%s' not found\n", name);
}

// Read file contents
void read_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use && strcmp(file_table[i].name, name) == 0) {
            kprintf("Contents of '%s': %s\n", name, file_table[i].data);
            return;
        }
    }
    kprintf("Error: File '%s' not found\n", name);
}

// Delete a file
void delete_file(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use && strcmp(file_table[i].name, name) == 0) {
            file_table[i].in_use = 0;
            file_table[i].size = 0;
            kprintf("File '%s' deleted\n", name);
            return;
        }
    }
    kprintf("Error: File '%s' not found\n", name);
}

// List all files
void list_files(void) {
    int found = 0;
    kprintf("Files in system:\n");
    for (int i = 0; i < MAX_FILES; i++) {
        if (file_table[i].in_use) {
            kprintf("- %s\n", file_table[i].name);
            found = 1;
        }
    }
    if (!found)
        kprintf("No files found.\n");
}
