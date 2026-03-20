[org 0x7c00]
KERNEL_OFFSET equ 0x1000 ; Kernel will be loaded here

    mov [BOOT_DRIVE], dl ; BIOS stores boot drive in DL

    ; Set up stack
    mov bp, 0x9000
    mov sp, bp

    mov bx, MSG_REAL_MODE
    call print_string

    call load_kernel     ; Load kernel from disk
    
    call set_video_mode  ; Switch to 320x200 VGA Mode 13h
    call switch_to_pm    ; Switch to 32-bit Protected Mode
    ; Note: switch_to_pm never returns

    jmp $

; --- 16-bit Routines ---

print_string:
    pusha
    mov ah, 0x0e
.loop:
    mov al, [bx]
    cmp al, 0
    je .done
    int 0x10
    inc bx
    jmp .loop
.done:
    popa
    ret

load_kernel:
    mov bx, MSG_LOAD_KERNEL
    call print_string

    ; Reset disk controller
    mov ah, 0x00
    mov dl, [BOOT_DRIVE]
    int 0x13
    jc disk_error

    ; Read sectors from disk
    mov bx, KERNEL_OFFSET      ; Destination buffer
    mov ah, 0x02               ; Function: Read Sectors
    mov al, 50                 ; Sector count
    mov dl, [BOOT_DRIVE]       ; Drive
    mov ch, 0x00               ; Cylinder
    mov dh, 0x00               ; Head
    mov cl, 0x02               ; Start Sector
    int 0x13

    jc disk_error
    cmp al, 50           ; Check if we read all sectors
    jne disk_error
    ret

disk_error:
    mov bx, MSG_DISK_ERROR
    call print_string
    jmp $

set_video_mode:
    mov ax, 0x13         ; VGA Mode 13h (320x200, 256 colors)
    int 0x10
    ret

switch_to_pm:
    cli                  ; Disable interrupts
    lgdt [gdt_descriptor]; Load GDT
    mov eax, cr0
    or eax, 0x1          ; Set PE (Protection Enable) bit
    mov cr0, eax
    jmp CODE_SEG:init_pm ; Far jump to 32-bit code

; --- GDT (Global Descriptor Table) ---

gdt_start:

gdt_null:                ; Mandatory null descriptor
    dd 0x0
    dd 0x0

gdt_code:                ; Code segment descriptor
    dw 0xffff            ; Limit (bits 0-15)
    dw 0x0               ; Base (bits 0-15)
    db 0x0               ; Base (bits 16-23)
    db 10011010b         ; 1st flags, type flags
    db 11001111b         ; 2nd flags, Limit (bits 16-19)
    db 0x0               ; Base (bits 24-31)

gdt_data:                ; Data segment descriptor
    dw 0xffff            ; Limit (bits 0-15)
    dw 0x0               ; Base (bits 0-15)
    db 0x0               ; Base (bits 16-23)
    db 10010010b         ; 1st flags, type flags
    db 11001111b         ; 2nd flags, Limit (bits 16-19)
    db 0x0               ; Base (bits 24-31)

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

; --- 32-bit Protected Mode ---

[bits 32]
init_pm:
    mov ax, DATA_SEG
    mov ds, ax
    mov ss, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov ebp, 0x90000
    mov esp, ebp

    call KERNEL_OFFSET   ; Jump to kernel entry point
    jmp $

; --- Data ---
BOOT_DRIVE db 0
MSG_REAL_MODE db " Booting AuroraOS...", 13, 10, 0
MSG_LOAD_KERNEL db " Loading Kernel...", 13, 10, 0
MSG_DISK_ERROR db " Disk Error!", 13, 10, 0

times 510-($-$$) db 0
dw 0xaa55