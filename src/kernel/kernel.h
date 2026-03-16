#ifndef KERNEL_H
#define KERNEL_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/* ── VGA text-mode ─────────────────────────────────────────────────── */
#define VGA_BASE        ((volatile uint16_t *)0xB8000)
#define VGA_COLS        80
#define VGA_ROWS        25
#define VGA_COLOR(fg,bg) ((uint8_t)((bg)<<4|(fg)))

typedef enum {
    VGA_BLACK=0, VGA_BLUE, VGA_GREEN, VGA_CYAN,
    VGA_RED, VGA_MAGENTA, VGA_BROWN, VGA_LIGHT_GREY,
    VGA_DARK_GREY, VGA_LIGHT_BLUE, VGA_LIGHT_GREEN, VGA_LIGHT_CYAN,
    VGA_LIGHT_RED, VGA_LIGHT_MAGENTA, VGA_LIGHT_BROWN, VGA_WHITE
} vga_color_t;

/* ── Terminal ──────────────────────────────────────────────────────── */
void term_init(void);
void term_clear(void);
void term_setcolor(uint8_t color);
void term_putchar(char c);
void term_write(const char *s);
void term_writeln(const char *s);
void term_printf(const char *fmt, ...);
void term_set_cursor(int x, int y);
int  term_get_row(void);

/* ── Memory ────────────────────────────────────────────────────────── */
#define PAGE_SIZE       4096
#define HEAP_START      0x400000   /* 4 MB */
#define HEAP_SIZE       (16*1024*1024)

void  mem_init(uint32_t mem_upper_kb);
void *kmalloc(size_t size);
void  kfree(void *ptr);
void  mem_stats(uint32_t *total, uint32_t *used, uint32_t *free_bytes);

/* ── String utilities ──────────────────────────────────────────────── */
size_t  kstrlen(const char *s);
int     kstrcmp(const char *a, const char *b);
int     kstrncmp(const char *a, const char *b, size_t n);
char   *kstrcpy(char *dst, const char *src);
char   *kstrncpy(char *dst, const char *src, size_t n);
char   *kstrcat(char *dst, const char *src);
char   *kstrchr(const char *s, int c);
char   *kstrstr(const char *hay, const char *needle);
void   *kmemset(void *dst, int c, size_t n);
void   *kmemcpy(void *dst, const void *src, size_t n);
int     kmemcmp(const void *a, const void *b, size_t n);
int     katoi(const char *s);
void    kitoa(int n, char *buf, int base);
char   *kstrtok(char *str, const char *delim, char **saveptr);
int     ksnprintf(char *buf, size_t size, const char *fmt, ...);

/* ── I/O ports ─────────────────────────────────────────────────────── */
static inline uint8_t  inb(uint16_t port) {
    uint8_t v; __asm__ volatile("inb %1,%0":"=a"(v):"Nd"(port)); return v;
}
static inline void outb(uint16_t port, uint8_t v) {
    __asm__ volatile("outb %0,%1"::"a"(v),"Nd"(port));
}
static inline void io_wait(void) { outb(0x80, 0); }

/* ── Interrupts / IDT ──────────────────────────────────────────────── */
void idt_init(void);
void pic_init(void);
void irq_install_handler(int irq, void (*handler)(void));
void sti(void);
void cli(void);

/* ── Keyboard ──────────────────────────────────────────────────────── */
void keyboard_init(void);
bool keyboard_has_data(void);
char keyboard_getchar(void);

/* ── Timer ─────────────────────────────────────────────────────────── */
void   timer_init(uint32_t hz);
uint64_t timer_ticks(void);
uint32_t timer_seconds(void);

/* ── VFS / Semantic FS ─────────────────────────────────────────────── */
#define VFS_MAX_NAME    64
#define VFS_MAX_PATH    256
#define VFS_MAX_NODES   512
#define VFS_MAX_DATA    8192
#define VFS_MAX_TAGS    8

typedef struct vfs_node {
    char     name[VFS_MAX_NAME];
    char     path[VFS_MAX_PATH];
    bool     is_dir;
    uint32_t size;
    uint8_t  data[VFS_MAX_DATA];
    uint32_t created;
    uint32_t modified;
    /* Semantic metadata */
    char     tags[VFS_MAX_TAGS][32];
    int      tag_count;
    char     activity[64];   /* which activity this belongs to */
    char     description[128];
    int      parent;
    bool     used;
} vfs_node_t;

void     vfs_init(void);
int      vfs_mkdir(const char *path);
int      vfs_create(const char *path);
int      vfs_write(const char *path, const void *data, uint32_t size);
int      vfs_read(const char *path, void *buf, uint32_t *size);
int      vfs_delete(const char *path);
int      vfs_find(const char *path);
void     vfs_ls(const char *path);
void     vfs_tag(const char *path, const char *tag);
void     vfs_set_activity(const char *path, const char *activity);
void     vfs_semantic_find(const char *query);  /* find files related to query */
void     vfs_activity_files(const char *activity);

/* ── Process Scheduler ─────────────────────────────────────────────── */
#define MAX_PROCS       32
#define PROC_NAME_LEN   32

typedef enum { PROC_READY, PROC_RUNNING, PROC_BLOCKED, PROC_DEAD } proc_state_t;

typedef struct process {
    uint32_t     pid;
    char         name[PROC_NAME_LEN];
    proc_state_t state;
    uint32_t     priority;
    uint32_t     cpu_time;
    uint32_t     created_at;
    uint32_t     memory_kb;
    char         activity[64];
} process_t;

void      sched_init(void);
uint32_t  sched_spawn(const char *name, uint32_t priority);
void      sched_kill(uint32_t pid);
void      sched_tick(void);
void      sched_list(void);
process_t *sched_current(void);
int       sched_count(void);

/* ── System Timeline ───────────────────────────────────────────────── */
#define TIMELINE_MAX    256
#define TIMELINE_MSG    128

typedef struct {
    uint32_t timestamp;
    char     message[TIMELINE_MSG];
    char     category[32];  /* kernel / user / lang / debug */
} timeline_event_t;

void timeline_init(void);
void timeline_record(const char *category, const char *msg);
void timeline_show(int last_n);
void timeline_show_range(uint32_t from, uint32_t to);
int  timeline_count(void);

/* ── Workspace Snapshots ───────────────────────────────────────────── */
#define MAX_SNAPSHOTS   16
#define SNAP_NAME_LEN   64

typedef struct {
    char     name[SNAP_NAME_LEN];
    uint32_t timestamp;
    char     open_files[8][VFS_MAX_PATH];
    int      open_file_count;
    char     active_activity[64];
    bool     used;
} snapshot_t;

void snapshot_save(const char *name);
bool snapshot_restore(const char *name);
void snapshot_list(void);

/* ── Activity Workspace ────────────────────────────────────────────── */
#define MAX_ACTIVITIES  16
#define ACT_NAME_LEN    64

typedef struct {
    char name[ACT_NAME_LEN];
    char description[128];
    char root_path[VFS_MAX_PATH];
    bool used;
} activity_t;

void activity_init(void);
void activity_create(const char *name, const char *desc);
void activity_open(const char *name);
void activity_list(void);
const char *activity_current(void);

/* ── System Monitoring ─────────────────────────────────────────────── */
void monitor_show_processes(void);
void monitor_show_memory(void);
void monitor_show_timeline(void);
void monitor_show_dashboard(void);

/* ── Settings ──────────────────────────────────────────────────────── */
typedef struct {
    bool  developer_mode;
    bool  learning_mode;
    bool  runtime_tracing;
    bool  predictive_alloc;
    bool  perf_optimization;
    int   visualization_level;  /* 0-3 */
    char  theme[32];
} aurora_settings_t;

extern aurora_settings_t g_settings;
void settings_init(void);
void settings_show(void);
void settings_set(const char *key, const char *value);

/* ── AuroraLang Runtime ────────────────────────────────────────────── */
void   aurora_runtime_init(void);
int    aurora_run_file(const char *path);
int    aurora_run_string(const char *code);
void   aurora_repl(void);

/* ── Shell ─────────────────────────────────────────────────────────── */
void shell_main(void);

/* ── Kernel entry ──────────────────────────────────────────────────── */
void kernel_main(uint32_t magic, void *mbi);

#endif /* KERNEL_H */
