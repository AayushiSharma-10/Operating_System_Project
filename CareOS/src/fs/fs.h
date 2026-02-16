#ifndef FS_H
#define FS_H

void fs_init();
int fs_find(const char *name);
void fs_write(const char *name, const char *data);
void fs_read(const char *name);
void fs_list();
void cmd_time();
void fs_delete(const char *name);
void fs_info(const char *name);
void fs_edit(const char *name);
void fs_search(const char *keyword);
void fs_rename(const char *old_name, const char *new_name);
void fs_export_all();
void fs_clear_all();
void fs_touch(const char *name);
void fs_cat(const char *name);
void fs_copy(const char *src, const char *dest);
void fs_move(const char *src, const char *dest);
void fs_mkdir(const char *dirname);
void fs_rmdir(const char *dirname);
void fs_cd(const char *path);
void fs_dir();

#endif