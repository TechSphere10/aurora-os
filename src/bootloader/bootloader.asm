[BITS 16]
[ORG 0x7C00]

start:
    ; Set up stack
    mov ax, 0x07C0
    add ax, 288
    mov ss, ax
    mov sp, 4096

    ; Set up data segment
    mov ax, 0x07C0
    mov ds, ax

    ; Print boot message
    mov si, boot_msg
    call print_string

    ; Load kernel (simplified - just jump to kernel location)
    ; In a real OS, you'd load from disk
    jmp 0x1000:0x0000

print_string:
    lodsb
    or al, al
    jz done
    mov ah, 0x0E
    int 0x10
    jmp print_string
done:
    ret

boot_msg db 'AuroraOS Bootloader v1.0', 0x0D, 0x0A, 'Initializing system...', 0x0D, 0x0A, 0

; Pad to 512 bytes and add boot signature
times 510 - ($ - $$) db 0
dw 0xAA55