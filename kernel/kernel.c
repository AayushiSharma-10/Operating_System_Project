#include "fs.h"
#include "kprintf.h"

void kmain() {
    kprintf("=== Mini OS: File Management Demo ===\n");

    // Create files
    create_file("file1.txt");
    create_file("file2.txt");

    // Write data
    write_file("file1.txt", "Hello, World!");
    write_file("file2.txt", "Welcome to Hume OS");

    // Read files
    read_file("file1.txt");
    read_file("file2.txt");

    // List files
    list_files();

    // Delete one file
    delete_file("file1.txt");

    // List again
    list_files();
}
