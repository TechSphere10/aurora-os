#include <stdint.h>
#include <stdbool.h>

// Forward declarations for kernel functions
void init_keyboard();
void init_memory();
bool keyboard_has_data();
char keyboard_buffer_get();
uint32_t get_total_memory();
uint32_t get_used_memory();
uint32_t get_free_memory();

// Forward declarations for new systems
void vfs_list_dir(const char *path);
void vfs_create_file(const char *path, const char *content);
void vfs_read_file(const char *path);
void scheduler_list_processes();
uint32_t sched_spawn(const char *name, uint32_t priority, uint32_t host_pid);
void desktop_show_info();
void services_log_message(const char *message);
void packages_list_installed();
void packages_install(const char *package_name);

// Shell functions
void execute_command(char *cmd_line);
#define MAX_CMD_LEN 256
#define MAX_ARGS 10

// Command definitions
typedef void (*command_func)(int argc, char *argv[]);

typedef struct {
    char *name;
    char *description;
    command_func func;
} command_t;

// Forward declarations
void cmd_help(int argc, char *argv[]);
void cmd_run(int argc, char *argv[]);
void cmd_deploy(int argc, char *argv[]);
void cmd_apps(int argc, char *argv[]);
void cmd_process(int argc, char *argv[]);
void cmd_memory(int argc, char *argv[]);
void cmd_analyze(int argc, char *argv[]);
void cmd_explain(int argc, char *argv[]);
void cmd_clear(int argc, char *argv[]);
void cmd_ls(int argc, char *argv[]);
void cmd_cat(int argc, char *argv[]);
void cmd_touch(int argc, char *argv[]);
void cmd_ps(int argc, char *argv[]);
void cmd_desktop(int argc, char *argv[]);
void cmd_services(int argc, char *argv[]);
void cmd_packages(int argc, char *argv[]);
void cmd_install(int argc, char *argv[]);
void cmd_symspawn(int argc, char *argv[]);

// Command table
command_t commands[] = {
    {"help", "Show available commands", cmd_help},
    {"run", "Execute application", cmd_run},
    {"deploy", "Deploy application to system desktop", cmd_deploy},
    {"apps", "List installed applications", cmd_apps},
    {"process", "Show running programs", cmd_process},
    {"memory", "Show memory usage", cmd_memory},
    {"analyze", "Analyze program structure", cmd_analyze},
    {"explain", "Explain programming concepts", cmd_explain},
    {"clear", "Clear screen", cmd_clear},
    {"ls", "List directory contents", cmd_ls},
    {"cat", "Display file contents", cmd_cat},
    {"touch", "Create empty file", cmd_touch},
    {"ps", "Show process list", cmd_ps},
    {"desktop", "Show desktop information", cmd_desktop},
    {"services", "Show system services", cmd_services},
    {"packages", "List installed packages", cmd_packages},
    {"install", "Install package", cmd_install},
    {"symspawn", "Spawn a symbiote process: symspawn <host_pid> <name>", cmd_symspawn},
    {NULL, NULL, NULL}
};

// Screen functions (simplified)
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
volatile char *screen = (volatile char*)0xB8000;

void print_char(char c, int x, int y, char color) {
    screen[(y * SCREEN_WIDTH + x) * 2] = c;
    screen[(y * SCREEN_WIDTH + x) * 2 + 1] = color;
}

void print_string(const char *str, int x, int y, char color) {
    for (int i = 0; str[i] != '\0'; i++) {
        print_char(str[i], x + i, y, color);
    }
}

void clear_screen() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT * 2; i++) {
        screen[i] = 0;
    }
}

// Command implementations
void cmd_help(int argc, char *argv[]) {
    print_string("Available Commands:", 0, 1, 0x07);
    int y = 2;
    for (int i = 0; commands[i].name != NULL; i++) {
        print_string(commands[i].name, 0, y, 0x07);
        print_string(" - ", 10, y, 0x07);
        print_string(commands[i].description, 13, y, 0x07);
        y++;
        if (y >= SCREEN_HEIGHT) break;
    }
}

void cmd_run(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: run <filename>", 0, 1, 0x07);
        return;
    }
    print_string("Running ", 0, 1, 0x07);
    print_string(argv[1], 8, 1, 0x07);
    // TODO: Implement actual program execution
}

void cmd_deploy(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: deploy <filename>", 0, 1, 0x07);
        return;
    }
    print_string("Deploying ", 0, 1, 0x07);
    print_string(argv[1], 10, 1, 0x07);
    print_string("...", 10 + strlen(argv[1]), 1, 0x07);
    // TODO: Implement deployment
}

void cmd_apps(int argc, char *argv[]) {
    print_string("Installed Applications:", 0, 1, 0x07);
    print_string("Calculator", 0, 2, 0x07);
    print_string("Notes", 0, 3, 0x07);
    // TODO: Dynamic app list
}

void cmd_process(int argc, char *argv[]) {
    print_string("PID   Application     Memory", 0, 1, 0x07);
    print_string("1     shell           2KB", 0, 2, 0x07);
    // TODO: Dynamic process list
}

void cmd_memory(int argc, char *argv[]) {
    uint32_t total = get_total_memory() / 1024;
    uint32_t used = get_used_memory() / 1024;
    uint32_t free = get_free_memory() / 1024;

    print_string("Total Memory: ", 0, 1, 0x07);
    print_number(total, 14, 1);
    print_string("KB", 18, 1, 0x07);

    print_string("Used Memory: ", 0, 2, 0x07);
    print_number(used, 14, 2);
    print_string("KB", 18, 2, 0x07);

    print_string("Free Memory: ", 0, 3, 0x07);
    print_number(free, 14, 3);
    print_string("KB", 18, 3, 0x07);

    // Visual memory layout
    print_string("Memory Layout:", 0, 5, 0x07);
    print_string("Kernel   ", 0, 6, 0x07);
    for (int i = 0; i < 7; i++) print_char(0xDB, 9 + i, 6, 0x07);
    print_string("Apps     ", 0, 7, 0x07);
    for (int i = 0; i < 5; i++) print_char(0xDB, 9 + i, 7, 0x07);
    print_string("Free     ", 0, 8, 0x07);
    for (int i = 0; i < 10; i++) print_char(0xDB, 9 + i, 8, 0x07);
}

void cmd_analyze(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: analyze <filename>", 0, 1, 0x07);
        return;
    }
    print_string("Analyzing ", 0, 1, 0x07);
    print_string(argv[1], 10, 1, 0x07);
    // TODO: Implement program analysis
}

void cmd_explain(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: explain <concept>", 0, 1, 0x07);
        return;
    }
    if (strcmp(argv[1], "loop") == 0) {
        print_string("A loop repeats a set of instructions multiple times.", 0, 1, 0x07);
        print_string("Example: for i from 1 to 5 print i", 0, 2, 0x07);
    } else {
        print_string("Concept not found", 0, 1, 0x07);
    }
}

void cmd_clear(int argc, char *argv[]) {
    clear_screen();
}

void cmd_ls(int argc, char *argv[]) {
    const char *path = (argc > 1) ? argv[1] : "/";
    print_string("Contents of ", 0, 1, 0x07);
    print_string(path, 12, 1, 0x07);
    print_string(":", 12 + strlen(path), 1, 0x07);
    vfs_list_dir(path);
}

void cmd_cat(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: cat <filename>", 0, 1, 0x07);
        return;
    }
    print_string("Contents of ", 0, 1, 0x07);
    print_string(argv[1], 13, 1, 0x07);
    print_string(":", 13 + strlen(argv[1]), 1, 0x07);
    vfs_read_file(argv[1]);
}

void cmd_touch(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: touch <filename>", 0, 1, 0x07);
        return;
    }
    vfs_create_file(argv[1], "");
    print_string("Created file: ", 0, 1, 0x07);
    print_string(argv[1], 14, 1, 0x07);
}

void cmd_ps(int argc, char *argv[]) {
    print_string("Process List:", 0, 1, 0x07);
    scheduler_list_processes();
}

void cmd_desktop(int argc, char *argv[]) {
    print_string("Desktop Information:", 0, 1, 0x07);
    desktop_show_info();
}

void cmd_services(int argc, char *argv[]) {
    print_string("System Services:", 0, 1, 0x07);
    services_log_message("Services status requested from shell");
    // TODO: Show actual services status
}

void cmd_packages(int argc, char *argv[]) {
    print_string("Installed Packages:", 0, 1, 0x07);
    packages_list_installed();
}

void cmd_install(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: install <package_name>", 0, 1, 0x07);
        return;
    }
    print_string("Installing ", 0, 1, 0x07);
    print_string(argv[1], 11, 1, 0x07);
    print_string("...", 11 + strlen(argv[1]), 1, 0x07);
    packages_install(argv[1]);
}

void cmd_symspawn(int argc, char *argv[]) {
    if (argc < 3) {
        print_string("Usage: symspawn <host_pid> <name>", 0, 1, 0x07);
        return;
    }
    uint32_t host_pid = atoi(argv[1]);
    const char* name = argv[2];
    uint32_t pid = sched_spawn(name, 1, host_pid);
    if (pid > 0) {
        print_string("Spawned symbiote '", 0, 1, 0x07);
        print_string(name, 19, 1, 0x07);
        print_string("' with PID ", 19 + strlen(name), 1, 0x07);
        print_number(pid, 19 + strlen(name) + 11, 1);
    }
}

// Shell main loop
void shell_main() {
    char cmd_buffer[MAX_CMD_LEN];
    int cursor_x = 0, cursor_y = 0;
    int buffer_pos = 0;

    clear_screen();
    print_string("AuroraOS > ", 0, 0, 0x07);
    cursor_y = 0;
    cursor_x = 11; // After "AuroraOS > "

    while (true) {
        // Check for keyboard input
        if (keyboard_has_data()) {
            char c = keyboard_buffer_get();

            if (c == '\n') {
                // Execute command
                cmd_buffer[buffer_pos] = '\0';
                execute_command(cmd_buffer);
                buffer_pos = 0;
                cursor_y++;
                if (cursor_y >= SCREEN_HEIGHT) {
                    cursor_y = 0;
                    clear_screen();
                }
                print_string("AuroraOS > ", 0, cursor_y, 0x07);
                cursor_x = 11;
            } else if (c == '\b') {
                // Backspace
                if (buffer_pos > 0) {
                    buffer_pos--;
                    cursor_x--;
                    print_char(' ', cursor_x, cursor_y, 0x07);
                }
            } else if (c >= 32 && c <= 126) {
                // Printable character
                if (buffer_pos < MAX_CMD_LEN - 1) {
                    cmd_buffer[buffer_pos++] = c;
                    print_char(c, cursor_x++, cursor_y, 0x07);
                }
            }
        }

        // Small delay
        for (volatile int i = 0; i < 10000; i++);
    }
}

// Execute command
void execute_command(char *cmd_line) {
    // Skip leading spaces
    while (*cmd_line == ' ') cmd_line++;

    if (*cmd_line == '\0') return;

    // Parse command and arguments
    char *argv[MAX_ARGS];
    int argc = 0;

    char *token = strtok(cmd_line, " ");
    while (token && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " ");
    }

    if (argc == 0) return;

    // Find and execute command
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            commands[i].func(argc, argv);
            return;
        }
    }

    // Command not found
    print_string("Command not found: ", 0, 1, 0x0C);
    print_string(argv[0], 18, 1, 0x0C);
}

void print_number(int num, int x, int y) {
    char buffer[16];
    int i = 0;

    if (num == 0) {
        print_char('0', x, y, 0x07);
        return;
    }

    while (num > 0) {
        buffer[i++] = '0' + (num % 10);
        num /= 10;
    }

    for (int j = i - 1; j >= 0; j--) {
        print_char(buffer[j], x++, y, 0x07);
    }
}

// Simple strcmp for shell
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

// Simple strlen
int strlen(const char *s) {
    int len = 0;
    while (s[len]) len++;
    return len;
}

// Simple atoi for shell commands
int atoi(const char *s) {
    int num = 0;
    while (*s >= '0' && *s <= '9') {
        num = num * 10 + (*s - '0');
        s++;
    }
    return num;
}