#ifndef COMMANDS_H
#define COMMANDS_H

void commands_execute(const char *cmd);
void cmd_meminfo(void);
void cmd_ps(void);
void cmd_test_memory(void);
void cmd_test_process(void);
void simple_split(const char *cmdline, char *cmd, char *arg1, char *arg2);

#endif
