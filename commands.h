#ifndef COMMANDS_H
#define COMMANDS_H

// Command functions
void cmd_help(void);
void cmd_clear(void);
void cmd_echo(const char* text);
void cmd_create(const char* filename);
void cmd_write(const char* args);
void cmd_read(const char* filename);
void cmd_delete(const char* filename);
void cmd_list(void);
void cmd_time(void);
void cmd_reboot(void);
void cmd_exit(void);
void cmd_meminfo(void);
void cmd_calc(const char* args);
// Add to existing commands.h
void cmd_meminfo();
void cmd_ps();
void cmd_test_memory();
void cmd_test_process();
// Utility functions
void parse_filename_and_text(const char* args, char* filename, char* text);
int string_to_int(const char* str);
void int_to_string(int num, char* buffer);

#endif