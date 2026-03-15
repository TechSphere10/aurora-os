# AuroraOS - Advanced Experimental Operating System

## Overview
AuroraOS is a comprehensive experimental operating system designed as a safe simulator with advanced features including a unified programming language, instant deployment, and self-explaining capabilities.

## Features

### Core Systems
- **Bootloader**: x86 assembly boot sequence
- **Kernel**: Core OS with memory management and device drivers
- **Shell**: Interactive command-line interface
- **AuroraLang**: Custom programming language with visual programming

### Advanced Features
- **Virtual File System (VFS)**: In-memory file system with directory operations
- **Process Scheduler**: Round-robin process management with context switching
- **Desktop GUI**: Text-based window system with focus management
- **System Services**: Logging, monitoring, and configuration management
- **Package Management**: Software installation, update, and removal system

### Safety & Simulation
- Completely safe simulator - no impact on host system
- Runs entirely within QEMU virtual machine
- All operations are contained within virtual environment

## Building & Running

### Prerequisites
- NASM (Netwide Assembler)
- GCC (MinGW for Windows)
- GNU ld linker
- QEMU emulator

### Build Process
```bash
# Run the build script
scripts/build.bat

# This will create:
# - build/bootloader.bin (boot sector)
# - build/kernel.bin (kernel image)
# - build/auroraos.img (complete OS image)
```

### Running
```bash
qemu-system-x86_64 build/auroraos.img
```

## Shell Commands

### Basic Commands
- `help` - Show available commands
- `clear` - Clear screen
- `memory` - Show memory usage

### File System Commands
- `ls [path]` - List directory contents
- `cat <file>` - Display file contents
- `touch <file>` - Create empty file

### Process Management
- `ps` - Show process list
- `process` - Show running programs

### System Management
- `desktop` - Show desktop information
- `services` - Show system services
- `packages` - List installed packages
- `install <package>` - Install package

### AuroraLang Commands
- `run <file>` - Execute AuroraLang program
- `deploy <file>` - Deploy application to desktop
- `analyze <file>` - Analyze program structure
- `explain <concept>` - Explain programming concepts

## Architecture

### Kernel Components
- `kernel.c` - Main kernel entry point and system initialization
- `memory.c` - Page-based memory allocator
- `keyboard.c` - PS/2 keyboard driver
- `vfs.c` - Virtual file system implementation
- `scheduler.c` - Process scheduler
- `desktop.c` - Desktop GUI system
- `services.c` - System services
- `packages.c` - Package management

### User Space
- `shell.c` - Command-line interface
- `auroralang.c` - Language interpreter
- `ai_assistant.c` - Code suggestions and optimization

## AuroraLang Programming Language

AuroraLang is a custom programming language designed for AuroraOS with the following features:

### Syntax Examples
```
# Visual Programming
draw circle at (100, 100) radius 50 color blue

# Live Coding
for i from 1 to 10:
    print "Number: " + i

# AI Assistance
suggest optimization for my_code
explain algorithm complexity
```

### Features
- Visual programming with graphical elements
- Live coding with instant feedback
- AI-powered code suggestions
- Self-explaining code comments
- Unified deployment system

## Development Status

### Completed Features
- ✅ Basic OS boot and initialization
- ✅ Memory management system
- ✅ Keyboard input handling
- ✅ Interactive shell
- ✅ AuroraLang interpreter foundation
- ✅ Virtual file system
- ✅ Process scheduler
- ✅ Desktop GUI framework
- ✅ System services
- ✅ Package management

### Future Enhancements
- Graphical desktop rendering
- Full AuroraLang syntax implementation
- Networking simulation
- Security features
- Additional device drivers

## Safety Notice

AuroraOS is designed as a **safe simulator only**. It:
- Runs entirely within QEMU virtualization
- Has no access to host system files or hardware
- Cannot modify your actual computer
- Is completely contained within the virtual machine

**Do not run AuroraOS outside of QEMU or on real hardware without proper testing.**

## License

This project is for educational and experimental purposes. Use at your own risk within the safe simulator environment.