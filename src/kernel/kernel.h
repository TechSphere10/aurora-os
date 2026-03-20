#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>

// Kernel Types
typedef struct process {
    uint32_t pid;
    uint32_t host_pid; // For Symbiosis
    char name[32];
    uint32_t esp;
    uint32_t stack_base;
    uint32_t state;
    uint32_t priority;
    struct process *next;
} process_t;

// Kernel API
void term_init();
void term_printf(const char *format, ...);
void term_writeln(const char *s);
void term_setcolor(uint8_t color);

uint32_t timer_seconds();
void timeline_record(const char *category, const char *message);

// Scheduler
uint32_t sched_spawn(const char *name, uint32_t priority, uint32_t host_pid);

// VFS
int vfs_mkdir(const char *path);
int vfs_delete(const char *path);

// String mappings (match string.h)
char *kitoa(int value, char *str, int base);
char *kstrtok(char *str, const char *delim);

// Colors
#define VGA_COLOR(fg, bg) ((bg << 4) | fg)
#define VGA_BLACK 0
#define VGA_LIGHT_GREY 7
#define VGA_LIGHT_GREEN 10
#define VGA_LIGHT_CYAN 11
#define VGA_LIGHT_MAGENTA 13

// Graphics API (VGA Mode 13h)
#define SCREEN_WIDTH_GFX 320
#define SCREEN_HEIGHT_GFX 200

void gfx_init(); // Shows splash screen
void gfx_clear_screen(uint8_t color);
void gfx_draw_pixel(int x, int y, uint8_t color);
void gfx_draw_char(int x, int y, char c, uint8_t color);

#endif