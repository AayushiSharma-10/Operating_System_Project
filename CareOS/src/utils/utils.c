#include <stdio.h>
#include "utils.h"
#include <stdlib.h>
#include "mm/mm.h"
#include <string.h>

char *safe_strdup(const char *src) {
    if (!src) return NULL;
    char *copy = malloc(strlen(src) + 1);
    if (copy) strcpy(copy, src);
    return copy;
}
void clear_screen() {
    printf("\033[2J\033[H");
}

void apply_alias(char *input) {
    if (!strcmp(input, "rm")) strcpy(input, "delete");
    else if (!strcmp(input, "cat")) strcpy(input, "read");
    else if (!strcmp(input, "vi")) strcpy(input, "edit");
}