#!/usr/bin/env bash
set -euo pipefail

BOOT=boot
KERN=kernel
IMG=os_image.img

echo "=== Hume OS Build Script ==="

# -------------------------------
# 1️⃣ Assemble bootloader
# -------------------------------
echo "[1] Assembling bootloader..."
nasm -f bin "$BOOT/stage1.asm" -o "$BOOT/stage1.bin"
nasm -f bin "$BOOT/stage2.asm" -o "$BOOT/stage2.bin"
echo "Bootloader assembled successfully."

# -------------------------------
# 2️⃣ Build kernel
# -------------------------------
echo "[2] Compiling kernel..."
gcc -m32 -ffreestanding -fno-pic -c "$KERN/kernel.c" -o "$KERN/kernel.o"
gcc -m32 -ffreestanding -fno-pic -c "$KERN/kprintf.c" -o "$KERN/kprintf.o"
gcc -m32 -ffreestanding -fno-pic -c "$KERN/fs.c" -o "$KERN/fs.o"
gcc -m32 -ffreestanding -fno-pic -c "$KERN/io.c" -o "$KERN/io.o"
as --32 "$KERN/boot.s" -o "$KERN/boot.o"
echo "Kernel compiled successfully."

# -------------------------------
# 3️⃣ Link kernel
# -------------------------------
echo "[3] Linking kernel..."
ld -m elf_i386 -Ttext 0x00100000 -o "$KERN/kernel.bin" \
   "$KERN/boot.o" "$KERN/kernel.o" "$KERN/kprintf.o" "$KERN/fs.o" "$KERN/io.o" \
   --oformat binary
echo "Kernel linked successfully."

# -------------------------------
# 4️⃣ Create disk image
# -------------------------------
echo "[4] Creating disk image..."
dd if=/dev/zero of="$IMG" bs=512 count=2880 status=none
dd if="$BOOT/stage1.bin" of="$IMG" conv=notrunc status=none
dd if="$BOOT/stage2.bin" of="$IMG" bs=512 seek=1 conv=notrunc status=none
dd if="$KERN/kernel.bin" of="$IMG" bs=512 seek=100 conv=notrunc status=none
echo "Disk image created: $IMG"

echo "=== Build complete ==="
