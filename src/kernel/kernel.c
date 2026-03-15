#include <stdint.h>

// Forward declarations
void shell_main();
void init_keyboard();
void init_memory();
void vfs_init();
void scheduler_init();
void desktop_init();
void services_init();
void packages_init();

// Simple kernel entry point
void kernel_main() {
    // Clear screen
    volatile char *video = (volatile char*)0xB8000;
    for (int i = 0; i < 80*25*2; i++) {
        video[i] = 0;
    }

    // Print welcome message
    const char *msg = "AuroraOS Kernel v1.0 - Initializing systems...";
    volatile char *screen = (volatile char*)0xB8000;
    for (int i = 0; msg[i] != '\0'; i++) {
        screen[i*2] = msg[i];
        screen[i*2+1] = 0x07; // White on black
    }

    // Initialize core systems
    init_memory();
    init_keyboard();
    vfs_init();
    scheduler_init();
    services_init();
    packages_init();

    // Print ready message
    const char *ready_msg = "AuroraOS Kernel v1.0 - System Ready.";
    for (int i = 0; ready_msg[i] != '\0'; i++) {
        screen[(1 * 80 + i) * 2] = ready_msg[i];
        screen[(1 * 80 + i) * 2 + 1] = 0x07;
    }

    // Initialize desktop
    desktop_init();

    // Start shell
    shell_main();

    // Infinite loop (should not reach here)
    while (1) {
        __asm__ volatile("hlt");
    }
}