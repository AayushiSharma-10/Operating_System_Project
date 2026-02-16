#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "fs.h"
#include "utils/utils.h"
#include "mm/mm.h"

#define MAX_FILES 100
#define MAX_CONTENT 2000

char *files[MAX_FILES];
char *file_data[MAX_FILES];
int file_count = 0;

void fs_init() {
    file_count = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        files[i] = NULL;
        file_data[i] = NULL;
    }

    DIR *dir = opendir(".");
    if (!dir) return;

    struct dirent *entry;
    while ((entry = readdir(dir)) && file_count < MAX_FILES) {
        struct stat st;
        if (stat(entry->d_name, &st) == 0 && S_ISREG(st.st_mode)) {
            FILE *fp = fopen(entry->d_name, "r");
            if (fp) {
                char buffer[MAX_CONTENT];
                size_t bytes = fread(buffer, 1, sizeof(buffer) - 1, fp);
                buffer[bytes] = '\0';
                fclose(fp);

                files[file_count] = mm_strdup(entry->d_name);
                file_data[file_count] = mm_strdup(buffer);
                file_count++;
            }
        }
    }
    closedir(dir);
}

int fs_find(const char *name) {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i] && strcmp(files[i], name) == 0)
            return i;
    }
    return -1;
}

void fs_write(const char *name, const char *data) {
    if (fs_find(name) >= 0) {
        printf("File already exists.\n");
        return;
    }

    if (file_count >= MAX_FILES) {
        printf("File system full.\n");
        return;
    }

    for (int i = 0; i < MAX_FILES; i++) {
        if (!files[i]) {
            files[i] = mm_strdup(name);
            file_data[i] = mm_strdup(data);
            file_count++;
            break;
        }
    }

    FILE *fp = fopen(name, "w");
    if (fp) {
        fputs(data, fp);
        fclose(fp);
        printf("File created.\n");
    } else {
        printf("Error: Could not write to disk.\n");
    }
}

void fs_read(const char *name) {
    int idx = fs_find(name);
    if (idx >= 0)
        printf("Content: %s\n", file_data[idx]);
    else
        printf("File not found.\n");
}

void fs_list() {
    printf("Files:\n");
    int found = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i]) {
            printf("- %s\n", files[i]);
            found = 1;
        }
    }
    if (!found) printf("No files found.\n");
}

void fs_delete(const char *name) {
    int idx = fs_find(name);
    if (idx >= 0) {
        remove(name);
        mm_free(files[idx]);
        mm_free(file_data[idx]);
        files[idx] = NULL;
        file_data[idx] = NULL;
        file_count--;
        printf("File deleted.\n");
    } else 
        printf("File not found.\n");
}

void fs_info(const char *name) {
    struct stat st;
    if (stat(name, &st) == 0) {
        printf("Filename: %s\n", name);
        printf("Size: %ld bytes\n", st.st_size);
        printf("Last modified: %s", ctime(&st.st_mtime));
    } else 
        printf("File not found.\n");
}

void fs_edit(const char *name) {
    int idx = fs_find(name);
    if (idx >= 0) {
        printf("Old Content: %s\n", file_data[idx]);
        printf("New Content: ");
        char temp[MAX_CONTENT];
        fgets(temp, sizeof(temp), stdin);
        temp[strcspn(temp, "\n")] = 0;

        mm_free(file_data[idx]);
        file_data[idx] = mm_strdup(temp);

        FILE *fp = fopen(name, "w");
        if (fp) {
            fputs(temp, fp);
            fclose(fp);
            printf("File updated.\n");
        } else 
            printf("Error: Could not write to disk.\n");
    } else 
        printf("File not found.\n");
}

void fs_search(const char *keyword) {
    printf("Searching for '%s'...\n", keyword);
    int found = 0;
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i] && file_data[i] && strstr(file_data[i], keyword)) {
            printf("- Found in '%s'\n", files[i]);
            found = 1;
        }
    }
    if (!found) printf("No matches found.\n");
}

void fs_rename(const char *old_name, const char *new_name) {
    int idx = fs_find(old_name);
    if (idx >= 0) {
        if (rename(old_name, new_name) == 0) {
            mm_free(files[idx]);
            files[idx] = mm_strdup(new_name);
            printf("File renamed.\n");
        } else {
            printf("Rename failed.\n");
        }
    } else {
        printf("File not found.\n");
    }
}

void fs_export_all() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i]) {
            FILE *fp = fopen(files[i], "w");
            if (fp) {
                fputs(file_data[i], fp);
                fclose(fp);
            }
        }
    }
    printf("All files exported to disk.\n");
}

void fs_clear_all() {
    for (int i = 0; i < MAX_FILES; i++) {
        if (files[i]) {
            remove(files[i]);
            mm_free(files[i]);
            mm_free(file_data[i]);
            files[i] = NULL;
            file_data[i] = NULL;
        }
    }
    file_count = 0;
    printf("All files cleared from memory and disk.\n");
}

void fs_touch(const char *name) {
    if (fs_find(name) >= 0) {
        printf("File already exists.\n");
        return;
    }

    fs_write(name, "");
}

void fs_cat(const char *name) {
    FILE *fp = fopen(name, "r");
    if (!fp) {
        printf("File not found: %s\n", name);
        return;
    }

    char buffer[MAX_CONTENT];
    while (fgets(buffer, sizeof(buffer), fp))
        printf("%s", buffer);
    fclose(fp);
    printf("\n");
}

void fs_copy(const char *src, const char *dest) {
    FILE *fp_src = fopen(src, "r");
    if (!fp_src) {
        printf("Source file not found: %s\n", src);
        return;
    }

    FILE *fp_dest = fopen(dest, "w");
    if (!fp_dest) {
        fclose(fp_src);
        printf("Failed to create destination file: %s\n", dest);
        return;
    }

    char buffer[MAX_CONTENT];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), fp_src)) > 0)
        fwrite(buffer, 1, bytes, fp_dest);

    fclose(fp_src);
    fclose(fp_dest);
    fs_write(dest, buffer); // update in-memory table
    printf("Copied '%s' to '%s'\n", src, dest);
}

void fs_move(const char *src, const char *dest) {
    fs_copy(src, dest);
    fs_delete(src);
}

void fs_mkdir(const char *dirname) {
    if (mkdir(dirname, 0755) == 0)
        printf("Directory created: %s\n", dirname);
    else
        perror("mkdir");
}

void fs_rmdir(const char *dirname) {
    if (rmdir(dirname) == 0)
        printf("Directory removed: %s\n", dirname);
    else
        perror("rmdir");
}

void fs_cd(const char *path) {
    if (chdir(path) == 0)
        printf("Changed directory to: %s\n", path);
    else
        perror("cd");
}