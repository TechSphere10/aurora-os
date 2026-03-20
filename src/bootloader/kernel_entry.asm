[bits 32]
[extern _kernel_main] ; Function in kernel.c (MinGW adds underscore)
[extern kernel_main]  ; Alternate name just in case

global _start
_start:
    ; Try calling kernel_main (some compilers prepend _)
    call _kernel_main
    jmp $