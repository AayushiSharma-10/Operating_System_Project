#!/bin/bash
set -e
echo "=== HumeOS fixed build script ==="

# tools check (optional)
# sudo apt update
# sudo apt install -y gcc-multilib nasm binutils build-essential qemu-system-x86

ROOT="$(pwd)"
BUILD="$ROOT/build"
rm -rf "$BUILD" os_image.img
mkdir -p "$BUILD"

echo "[1] Assemble stage1 (MBR 512-byte)"
nasm -f bin boot/stage1.asm -o "$BUILD/stage1.bin"

# copy stage2 template to build; we'll patch it later with correct sector count
cp boot/stage2.asm "$BUILD/stage2.asm"

echo "[2] Assemble kernel entry (boot.s) -> build/boot.o"
# Try nasm for intel syntax
nasm -f elf32 kernel/boot.s -o "$BUILD/boot.o"

echo "[3] Compile C sources -> build/*.o"
for f in kernel/*.c mm/*.c process/*.c; do
  [ -f "$f" ] || continue
  echo "  compiling $f"
  gcc -m32 -ffreestanding -c "$f" -I. -o "$BUILD/$(basename "$f" .c).o"
done

# also compile any additional kernel .s/.asm files (skip boot.s)
for f in kernel/*.s kernel/*.asm; do
  [ -f "$f" ] || continue
  if [ "$(basename "$f")" = "boot.s" ]; then
    continue
  fi
  nasm -f elf32 "$f" -o "$BUILD/$(basename "$f" .s).o"
done

echo "[4] Link all objects into an ELF (kernel.elf) at 0x10000"
# ensure linker.ld has ENTRY(_start) and origin 0x10000
ld -m elf_i386 -T linker.ld -o "$BUILD/kernel.elf" "$BUILD"/*.o

echo "[5] Create flat binary kernel (build/kernel.bin)"
objcopy -O binary "$BUILD/kernel.elf" "$BUILD/kernel.bin"

# compute sectors required by kernel
KERNEL_BYTES=$(stat -c%s "$BUILD/kernel.bin")
KERNEL_SECTORS=$(( (KERNEL_BYTES + 511) / 512 ))
echo "  kernel size: $KERNEL_BYTES bytes -> $KERNEL_SECTORS sectors"

echo "[6] Patch stage2 with computed KERNEL_SECTORS and assemble"
# replace the 'KERNEL_SECTORS equ N' line in stage2.asm (works for your stage2 style)
sed "s/KERNEL_SECTORS equ [0-9]\\+/KERNEL_SECTORS equ $KERNEL_SECTORS/" "$BUILD/stage2.asm" > "$BUILD/stage2_patched.asm"
nasm -f bin "$BUILD/stage2_patched.asm" -o "$BUILD/stage2.bin"

echo "[7] Create raw disk image and write sectors (sector0=stage1, sector1=stage2, sector2..=kernel)"
TOTAL_SECTORS=$(( 2 + KERNEL_SECTORS ))
echo "  total sectors: $TOTAL_SECTORS"
dd if=/dev/zero of=os_image.img bs=512 count=$TOTAL_SECTORS status=none

# stage1 -> sector 0
dd if="$BUILD/stage1.bin" of=os_image.img bs=512 count=1 conv=notrunc status=none

# stage2 -> sector 1
dd if="$BUILD/stage2.bin" of=os_image.img bs=512 seek=1 conv=notrunc status=none

# kernel -> starting at sector 2
dd if="$BUILD/kernel.bin" of=os_image.img bs=512 seek=2 conv=notrunc status=none

# optional: pad to 1.44MB (floppy) if you want; not strictly necessary
# dd if=/dev/zero bs=512 count=$((2880 - TOTAL_SECTORS)) >> os_image.img

echo "[8] Build finished: os_image.img (size $(stat -c%s os_image.img) bytes)"

echo "[9] Run QEMU (BIOS boot path). Press Ctrl+C to quit."
qemu-system-i386 -drive format=raw,file=os_image.img -m 512M -serial stdio
