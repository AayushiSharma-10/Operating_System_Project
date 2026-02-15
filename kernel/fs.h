#ifndef FS_H
#define FS_H

void create_file(const char *name);
void write_file(const char *name, const char *data);
char* read_file(const char *name);
void delete_file(const char *name);
void list_files(void);

#endif
