# User Manual

## Overview

This repository appears to contain components of an operating system project, including boot, kernel, filesystem, memory management, process management modules, CLI utilities, build scripts, and documentation resources.

---

## Repository Structure

### **boot/**

Contains bootloader-related code or scripts responsible for initializing the system.

### **docs/**

Documentation files and resources used for proposing or describing project components.

### **fs/**

Source code for the file system layer.

### **kernel/**

Core operating system kernel code, including system initialization and management routines.

### **mm/**

Memory management modules, which may include allocation, paging, or segmentation implementations.

### **process/**

Process management utilities, likely involving task scheduling, context switching, and process control structures.

### **build.sh**

A shell script used to build the OS image and compile system components.

### **cli.c / cli.h**

Source code and header for the command-line interface implementation of the OS.

### **commands.c / commands.h**

Function definitions and headers for built-in OS commands.

### **os_image.img**

Compiled OS binary image generated during the build process.

### **ppt.pptx**

Presentation file, possibly used for demonstrating or pitching the OS project.

### **README.md**

Main project readme file.

### **Tasks.docx**

Document outlining tasks or project milestones.

### **Temp_Project_Proposal...**

Project proposal document.

---

## Build Instructions

1. Ensure required tools are installed (e.g., GCC, Make, QEMU or Bochs).
2. Run the build script:

   ```bash
   ./build.sh
   ```
3. After building, the output OS image will be located at `os_image.img`.

---

## Running the OS

Once the OS is running, you can interact with it through the built-in command-line shell.

---

## Command-Line Shell (CMD) Usage

The OS includes a simple command-line interface implemented in `cli.c` and `commands.c`. Below is a user manual for using the shell.

### **Available Commands**

(These may vary depending on your implementation, update as needed.)

#### **1. help**

Displays a list of available commands.

```
help
```

#### **2. clear**

Clears the terminal screen.

```
clear
```

#### **3. echo**

Prints text to the screen.

```
echo Hello World
```

#### **4. reboot / shutdown**

System control commands (only if implemented).

```
reboot
shutdown
```

#### **5. fs commands** (if supported)

```
ls       # list directory contents
cat file # display file contents
```

### **Shell Behavior**

* Commands are parsed in `cli.c`.
* Actual command logic lives in `commands.c`.
* Unknown commands produce an error message.

### **Creating New Commands**

To add a new command:

1. Add its prototype in `commands.h`.
2. Implement the function in `commands.c`.
3. Register the command inside the command table.

Example format:

```c
int mycmd(int argc, char** argv) {
    // Your command logic
    return 0;
}
```

---

Use an emulator such as QEMU:

```bash
qemu-system-x86_64 -drive format=raw,file=os_image.img
```

---

## Code Components

### **CLI Module**

* `cli.c` contains the logic for parsing and executing user commands.
* `commands.c` includes implementation for built-in commands.

### **Kernel Module**

* Handles system boot, initialization of subsystems, interrupt setup, and essential kernel services.

### **Filesystem Module**

* Responsible for mounting, reading, and writing files.

### **Memory Management Module**

* Likely includes dynamic memory allocation and management of physical/virtual memory.

### **Process Management Module**

* Includes scheduling, context switching, PCB (Process Control Block), and process lifecycle.

---

## Extending the System

* Add new commands via `commands.c` and `commands.h`.
* Add kernel features in `kernel/`.
* Improve memory or process management by modifying `mm/` and `process/`.

---

## Troubleshooting

* If the OS fails to boot, check bootloader configuration in `boot/`.
* If build fails, ensure proper permissions and compiler toolchain.
* For CLI errors, verify command definitions.

---

## Additional Resources

Refer to the documents in the `docs/` directory for detailed descriptions, proposals, and reports.

---

_## Command Reference Manual

Below is the detailed user manual for all commands defined in `commands.h`. This section is suitable for displaying directly inside your OS shell using the `help` command.

---

# **Shell Command Manual**

This operating system provides a built‑in command-line interface. The following commands are available:

---

## **help**

Displays all available commands.

```
help
```

---

## **clear**

Clears the screen.

```
clear
```

---

## **echo**

Prints text to the screen.

```
echo Hello World
```

---

## **create**

Creates a new empty file in the filesystem.

```
create <filename>
```

Example:

```
create notes.txt
```

---

## **write**

Writes text to a file. If the file doesn't exist, create it first.

```
write <filename> <text>
```

Example:

```
write notes.txt This is a sample note
```

---

## **read**

Displays the contents of a file.

```
read <filename>
```

---

## **delete**

Deletes a file.

```
delete <filename>
```

---

## **list**

Lists all files in the filesystem.

```
list
```

---

## **time**

Shows the current system time.

```
time
```

---

## **reboot**

Reboots the operating system.

```
reboot
```

---

## **exit**

Exits the shell or shuts down the system (depending on implementation).

```
exit
```

---

## **meminfo**

Displays memory usage information.

```
meminfo
```

This will typically show:

* Total memory
* Used memory
* Free memory
* Allocation statistics

---

## **ps**

Shows the list of running processes.

```
ps
```

Expected output:

* PID
* Process name
* State

---

## **test_memory**

Runs internal memory subsystem tests.

```
test_memory
```

Used for debugging your memory manager.

---

## **test_process**

Runs process creation, scheduling, and management tests.

```
test_process
```

Helpful for validating process subsystem behavior.

---

## **calc**

Evaluates a simple math expression.

```
calc <expression>
```

Examples:

```
calc 2+2
calc 10-3
calc 4*8
calc 20/5
```

---

## **Utility Functions (Internal Use)**

These are used by your command parser:

### `parse_filename_and_text(args, filename, text)`

Splits input into `<filename>` and `<text>`.

### `string_to_int(str)`

Converts string to integer.

### `int_to_string(num, buffer)`

Converts integer to string (for printing).

---

# End of User Manual_
