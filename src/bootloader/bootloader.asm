; AuroraOS Multiboot Bootloader
; Compliant with Multiboot Specification 1 (GRUB compatible)

BITS 32

; Multiboot header constants
MULTIBOOT_MAGIC     equ 0x1BADB002
MULTIBOOT_FLAGS     equ 0x00000003   ; align modules + memory map
MULTIBOOT_CHECKSUM  equ -(MULTIBOOT_MAGIC + MULTIBOOT_FLAGS)

; Kernel stack size
STACK_SIZE equ 0x4000   ; 16KB

section .multiboot
align 4
    dd MULTIBOOT_MAGIC
    dd MULTIBOOT_FLAGS
    dd MULTIBOOT_CHECKSUM

section .bss
align 16
stack_bottom:
    resb STACK_SIZE
stack_top:

section .text
global _start
extern kernel_main

_start:
    ; Set up stack
    mov esp, stack_top

    ; Push multiboot info pointer and magic for kernel
    push ebx        ; multiboot info struct pointer
    push eax        ; multiboot magic number

    ; Call kernel main
    call kernel_main

    ; Should never return - halt
.hang:
    cli
    hlt
    jmp .hang
