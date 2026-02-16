# Makefile for MiniOS (inside boot/)

CC      = gcc
LD      = ld
NASM    = nasm
CFLAGS  = -ffreestanding -m32 -nostdlib -fno-pic -fno-pie -Wall -Wextra -I./src
LDFLAGS = -T link.ld
BUILD   = build

OBJS = \
	$(BUILD)/fs.o \
	$(BUILD)/mm.o \
	$(BUILD)/proc.o \
	$(BUILD)/shell.o \
	$(BUILD)/utils.o \
	$(BUILD)/main.o \
	$(BUILD)/minilib.o \
	$(BUILD)/input.o

# Ensure build directory exists
$(shell mkdir -p $(BUILD))

# Bootloader and optional loader
bootloader.bin: bootloader.asm
	$(NASM) -f bin $< -o $@

loader.bin: loader.asm
	$(NASM) -f bin $< -o $@

# C modules
$(BUILD)/fs.o: src/fs/fs.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/mm.o: src/mm/mm.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/proc.o: src/proc/proc.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/shell.o: src/shell/shell.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/utils.o: src/utils/utils.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/main.o: src/main.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/minilib.o: src/libc/minilib.c
	$(CC) $(CFLAGS) -c $< -o $@

$(BUILD)/input.o: src/input.c
	$(CC) $(CFLAGS) -c $< -o $@

# Link shell logic
shell.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o $@ $(OBJS)

# Final bootable image
minios.img: bootloader.bin shell.bin
	cat bootloader.bin shell.bin > minios.img

# Clean build artifacts
clean:
	rm -f bootloader.bin loader.bin shell.bin minios.img $(BUILD)/*.o

.PHONY: clean