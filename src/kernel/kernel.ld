ENTRY(kernel_main)

SECTIONS {
    . = 0x10000; /* Load at 0x1000:0x0000 */

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}