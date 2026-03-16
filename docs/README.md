PROJECT VISION

Design and implement a research-level operating system called AuroraOS along with a custom programming language called AuroraLang. The goal is to explore new computing paradigms rather than replicate existing operating systems.

AuroraOS must introduce innovative system architecture, developer-centric workflows, semantic data organization, intelligent system monitoring, and seamless integration with a new programming language ecosystem.

The system must run like a real operating system but be tested safely inside a virtual machine environment so it does not interfere with the host operating system.

Development Environment

Code Editor: Visual Studio Code
OS Emulator: QEMU
Bootloader: GRUB
Kernel Language: C or Rust
Programming Language Implementation: Python or Rust
Build Tools: Makefile or CMake

AuroraOS must be built as a bootable ISO image and executed using QEMU.

Example execution command:

qemu-system-x86_64 -cdrom auroraos.iso

SYSTEM ARCHITECTURE

Hardware
Bootloader (GRUB)
AuroraOS Kernel
System Services
AuroraLang Runtime
Applications
User Interface

BOOT PROCESS

1. System starts and GRUB loads the AuroraOS kernel.
2. Kernel initializes memory management and hardware drivers.
3. File system mounts and system services start.
4. AuroraLang runtime initializes.
5. User interface launches and user workspace becomes available.

KERNEL RESPONSIBILITIES

The kernel must implement fundamental operating system functionality including:

Process Management
Memory Management
CPU Scheduling
Interrupt Handling
Device Drivers
File System Management
System Calls
Resource Allocation

FILE SYSTEM DESIGN

AuroraOS should implement a Semantic File System.

Instead of rigid folder hierarchies, files are organized by meaning, relationships, and context.

Example interaction:

find files related to "AI project"

The system automatically returns related code, notes, datasets, documents, and logs.

ACTIVITY-CENTRIC WORKSPACE

The OS interface should revolve around activities instead of applications.

Example activity:

AI Research

Inside this activity:

code
datasets
research papers
notes
experiments

All resources automatically group around the activity context.

INTENT-BASED COMMAND SYSTEM

Users interact with the OS by describing their goals.

Examples:

backup my project
clean unused files
optimize system performance
open yesterday's work

AuroraOS interprets the intention and executes appropriate system actions.

SYSTEM TIMELINE

AuroraOS records system events in a chronological timeline.

Example:

10:00 project opened
10:05 program compiled
10:06 program crashed
10:07 debugging session started

This allows developers to inspect system history and replay events.

TIME-TRAVEL DEBUGGING

Programs can be rewound and replayed.

Example command:

rewind program 5 seconds

This helps developers analyze failures and debugging scenarios.

VISUAL SYSTEM MONITORING

AuroraOS includes a visualization engine displaying system activity through interactive dashboards.

Visualizations include:

process tree
memory allocation
disk activity
network traffic
CPU scheduling

WORKSPACE SNAPSHOTS

Users can save system states.

Example:

snapshot "AI work session"

Later restore:

restore "AI work session"

This restores open applications, files, and system state.

AUTOMATIC DEVELOPMENT ENVIRONMENTS

When a project is created:

create project AI

AuroraOS automatically generates:

project directory structure
dependency configuration
runtime environment
build scripts

DEVELOPER-FIRST OPERATING SYSTEM

AuroraOS integrates built-in development tools including:

code editor
debugger
compiler
package manager
runtime visualizer
documentation viewer

INTELLIGENT COMMAND DISCOVERY

AuroraOS suggests commands dynamically as the user types.

Example:

open

Suggestions appear:

open project
open file
open last workspace

ADVANCED SETTINGS SYSTEM

AuroraOS must include a comprehensive settings interface.

Categories include:

System Intelligence
Developer Mode
Security and Privacy
Resource Optimization
Visualization
Workspace Management
AI Assistance

Example settings options:

Predictive resource allocation
Learning mode
Runtime tracing
Performance optimization
Visualization configuration

PRIVACY DASHBOARD

Displays:

application permissions
network usage
file access history

USER INTERFACE PHILOSOPHY

AuroraOS must avoid resembling existing operating systems.

Instead implement:

activity-centric navigation
command palette interaction
project-focused workspace

PROGRAMMING LANGUAGE: AURORALANG

AuroraLang is a modern programming language designed to overcome common issues present in languages like Python, JavaScript, and C.

Language Goals

Readable syntax
Strong typing
Memory safety
Built-in concurrency
Helpful error messages
Integrated dependency management

AuroraLang must support both frontend and backend development.

LANGUAGE IMPLEMENTATION ARCHITECTURE

AuroraLang must include the following components:

Lexer
Parser
Abstract Syntax Tree
Interpreter
Compiler
Runtime Environment

PROGRAMMING LANGUAGE SYNTAX COMPONENTS

Variables

let x = 10

Constants

const PI = 3.14

Functions

function add(a,b){
return a+b
}

Control Flow

if x > 10 {
print "large"
}

Loops

loop i from 1 to 10 {
print i
}

Error Handling

try {
open file
}
catch {
print "file not found"
}

CONCURRENCY

AuroraLang should support simple parallel execution.

Example:

parallel {
task1()
task2()
}

FILE SYSTEM INTERACTION

create file "data.txt"
write "hello" to file

OPERATING SYSTEM INTERACTION

system.memory()
system.processes()
system.disk_usage()

FRONTEND DEVELOPMENT

AuroraLang should support UI creation.

Example:

ui window {
title "Aurora App"

button "Click" {
print "Hello"
}
}

BACKEND DEVELOPMENT

AuroraLang should support server creation.

Example:

server start 8080

route "/hello" {
return "Hello World"
}

DEDICATED RUNTIME APPLICATION

AuroraLang programs run using a runtime command.

Example:

aurora run program.al

Runtime responsibilities:

parse code
compile or interpret code
execute program
handle runtime errors

BUILT-IN PACKAGE MANAGER

Example usage:

use web
use graphics
use database

Dependencies install automatically.

SMART ERROR MESSAGES

Example output:

Error on line 12
Expected closing bracket
Suggestion: add "}"

PROGRAMMING LANGUAGE TOOLCHAIN

AuroraLang ecosystem tools should include:

package manager
code formatter
debugger
testing framework
documentation generator

PROJECT DIRECTORY STRUCTURE

AuroraOS

bootloader
kernel
filesystem
scheduler
drivers
system services

auroralang
lexer
parser
interpreter
compiler

apps
shell
ui
settings
visualization

tools
build

BUILD PROCESS

Compile kernel
Generate ISO image

Run the OS inside QEMU:

qemu-system-x86_64 -cdrom auroraos.iso

FINAL DELIVERABLES

Bootable operating system
Custom kernel
Semantic file system
Activity-centric workspace
Custom programming language
Interpreter implementation
Compiler implementation
Runtime execution environment
Visualization engine
Advanced settings system
Developer tools

AuroraOS must demonstrate original system concepts rather than replicate the design of Windows, Linux, or macOS.