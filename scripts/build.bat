@echo off
setlocal enabledelayedexpansion

echo [AURORAOS BUILD SYSTEM]
echo -----------------------

:: Create build folder
if not exist build mkdir build

:: --- Check Tools ---
where gcc >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] GCC not found. Add it to PATH.
    exit /b 1
)

where nasm >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] NASM not found. Add it to PATH.
    exit /b 1
)

where ld >nul 2>nul
if %errorlevel% neq 0 (
    echo [ERROR] LD not found. Add it to PATH.
    exit /b 1
)

:: --- 1. Build Bootloader ---
echo [1/4] Building Bootloader...

if not exist src\bootloader\kernel_entry.asm (
    echo [ERROR] src\bootloader\kernel_entry.asm not found.
    exit /b 1
)
nasm -f win32 src\bootloader\kernel_entry.asm -o build\kernel_entry.o
if errorlevel 1 exit /b 1

:: --- 2. Compile Kernel & Modules ---
echo [2/4] Compiling System Components...

set CFLAGS=-m32 -ffreestanding -nostdlib -fno-builtin -fno-stack-protector -I src

set OBJ_FILES=

for /R src %%f in (*.c) do (
    echo   Compiling %%~nxf...
    gcc %CFLAGS% -c "%%f" -o "build\%%~nf.o"
    if errorlevel 1 (
        echo [ERROR] Compilation failed for %%~nxf
        exit /b 1
    )
    set OBJ_FILES=!OBJ_FILES! build\%%~nf.o
)

:: --- 3. Link Kernel ---
echo [3/4] Linking Kernel...

:: Note: Linking kernel_entry.o FIRST is critical to ensure execution starts there.
:: Using i386pe for Windows MinGW compatibility.
ld -m i386pe -Ttext 0x1000 -o build\kernel.bin build\kernel_entry.o !OBJ_FILES! --oformat binary

if errorlevel 1 (
    echo [ERROR] Linking failed
    exit /b 1

:: --- 4. Create OS Image ---
echo [4/4] Creating OS Image...

copy /b build\bootloader.bin + build\kernel.bin build\auroraos.img >nul

if errorlevel 1 (
    echo [ERROR] Failed to create OS image
    exit /b 1
)

echo -----------------------
echo [SUCCESS] Build Complete: build\auroraos.img
echo Run with:
echo qemu-system-x86_64 -drive format=raw,file=build\auroraos.img