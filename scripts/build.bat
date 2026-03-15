@echo off
echo Building AuroraOS...

REM Build bootloader
nasm -f bin src\bootloader\bootloader.asm -o build\bootloader.bin

REM Build kernel and all components
gcc -ffreestanding -c src\kernel\kernel.c -o build\kernel.o
gcc -ffreestanding -c src\kernel\keyboard.c -o build\keyboard.o
gcc -ffreestanding -c src\kernel\memory.c -o build\memory.o
gcc -ffreestanding -c src\shell\shell.c -o build\shell.o
gcc -ffreestanding -c src\auroralang\auroralang.c -o build\auroralang.o
gcc -ffreestanding -c src\auroralang\ai_assistant.c -o build\ai_assistant.o
gcc -ffreestanding -c src\kernel\vfs.c -o build\vfs.o
gcc -ffreestanding -c src\kernel\scheduler.c -o build\scheduler.o
gcc -ffreestanding -c src\kernel\desktop.c -o build\desktop.o
gcc -ffreestanding -c src\kernel\services.c -o build\services.o
gcc -ffreestanding -c src\kernel\packages.c -o build\packages.o
ld -T src\kernel\kernel.ld -o build\kernel.bin build\kernel.o build\keyboard.o build\memory.o build\shell.o build\auroralang.o build\ai_assistant.o build\vfs.o build\scheduler.o build\desktop.o build\services.o build\packages.o

REM Combine bootloader and kernel into disk image
copy /b build\bootloader.bin + build\kernel.bin build\auroraos.img

echo Build complete. Run with: qemu-system-x86_64 build\auroraos.img