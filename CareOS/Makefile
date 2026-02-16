# Compiler and flags
CC = gcc
PKG_CFLAGS := $(shell pkg-config --cflags libnotify 2>/dev/null)
PKG_LIBS := $(shell pkg-config --libs libnotify 2>/dev/null)

CFLAGS = -Wall -I./src $(PKG_CFLAGS)
ifeq ($(strip $(PKG_LIBS)),)
else
CFLAGS += -DHAVE_LIBNOTIFY
endif
LDFLAGS = $(PKG_LIBS)

# Build directories
OBJDIR = build

# Object files for MiniOS
OBJS = $(addprefix $(OBJDIR)/, \
    main.o \
    shell/shell.o shell/history.o \
    proc/proc.o fs/fs.o mm/mm.o utils/utils.o)

# Object files for standalone shell
SHELL_OBJS = $(addprefix $(OBJDIR)/, \
    shell/shell_main.o \
    shell/shell.o shell/history.o \
    proc/proc.o fs/fs.o mm/mm.o utils/utils.o)

# Default target
all: MiniOS shell

# Build MiniOS
MiniOS: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Build standalone shell binary
shell: $(SHELL_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

# Pattern rule for compiling .c to .o
$(OBJDIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Special rule for shell_main.c with STANDALONE_SHELL
$(OBJDIR)/shell/shell_main.o: src/shell/shell_main.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -DSTANDALONE_SHELL -c $< -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) MiniOS shell

.PHONY: all clean