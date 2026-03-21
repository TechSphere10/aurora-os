#include <stdint.h>
#include <stdbool.h>
#include "../auroralang/string.h"
#include "../kernel/kernel.h"
#include "../auroralang/auroralang.h"

// Forward declarations for AI system
void ai_log_event(int type, const char* data);
void ai_generate_code_from_intent(const char* intent, char* code_out, size_t out_size);
void ai_explain_topic(const char *topic);
void analyze_code(const char *code);

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

// --- Self-Improving Language: Aliasing ---
#define MAX_ALIASES 50
typedef struct {
    char name[32];
    char value[128];
} alias_t;

alias_t aliases[MAX_ALIASES];
int alias_count = 0;

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
void cmd_lsnodes(int argc, char *argv[]);
void cmd_connect(int argc, char *argv[]);
void cmd_alias(int argc, char *argv[]);
void cmd_do(int argc, char *argv[]);
void print_number(int num, int x, int y);

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
    {"lsnodes", "List all UI nodes on the desktop canvas", cmd_lsnodes},
    {"connect", "Connect two UI nodes: connect <id1> <id2>", cmd_connect},
    {"alias", "Create a command alias: alias name=\"command\"", cmd_alias},
    {"do", "Execute an intent: do \"your goal\"", cmd_do},
    {NULL, NULL, NULL}
};

// Screen functions (simplified)
// In Graphics Mode 13h (320x200), using 8x8 font:
// Width: 320/8 = 40 cols
// Height: 200/8 = 25 rows
#define SCREEN_WIDTH 40
#define SCREEN_HEIGHT 25

void print_char(char c, int x, int y, char color) {
    // Use graphics rendering
    gfx_draw_char(x * 8, y * 8, c, (uint8_t)color);
}

void print_string(const char *str, int x, int y, char color) {
    for (int i = 0; str[i] != '\0'; i++) {
        print_char(str[i], x + i, y, color);
    }
}

void clear_screen() {
    gfx_clear_screen(0); // Clear to black
}

// Initialize shell logic
void shell_init() {
    gfx_init(); // Draw startup splash
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
    
    char buf[1024];
    uint32_t sz = sizeof(buf) - 1;
    if (vfs_read(argv[1], buf, &sz) == 0) {
        buf[sz] = '\0';
        aurora_run_string(buf);
    } else {
        print_string("Error: Could not read file.", 0, 2, 0x0C);
    }
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
    uint32_t total_bytes, used_bytes, free_bytes;
    mem_stats(&total_bytes, &used_bytes, &free_bytes);
    uint32_t total = total_bytes / 1024;
    uint32_t used = used_bytes / 1024;
    uint32_t free = free_bytes / 1024;

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
    
    char buf[1024];
    uint32_t sz = sizeof(buf) - 1;
    if (vfs_read(argv[1], buf, &sz) == 0) {
        buf[sz] = '\0';
        analyze_code(buf);
    } else {
        print_string("Error: Could not read file.", 0, 2, 0x0C);
    }
}

void cmd_explain(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: explain <concept>", 0, 1, 0x07);
        return;
    }
    ai_explain_topic(argv[1]);
}

void cmd_clear(int argc, char *argv[]) {
    clear_screen();
}

void cmd_ls(int argc, char *argv[]) {
    const char *path = (argc > 1) ? argv[1] : "/";
    print_string("Contents of ", 0, 1, 0x07);
    print_string(path, 12, 1, 0x07);
    print_string(":", 12 + strlen(path), 1, 0x07);
    vfs_ls(path);
}

void cmd_cat(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: cat <filename>", 0, 1, 0x07);
        return;
    }
    print_string("Contents of ", 0, 1, 0x07);
    print_string(argv[1], 13, 1, 0x07);
    print_string(":", 13 + strlen(argv[1]), 1, 0x07);
    
    char buf[1024];
    uint32_t sz = sizeof(buf) - 1;
    if (vfs_read(argv[1], buf, &sz) == 0) {
        buf[sz] = '\0';
        print_string(buf, 0, 2, 0x07);
    } else {
        print_string("Error reading file", 0, 2, 0x0C);
    }
}

void cmd_touch(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: touch <filename>", 0, 1, 0x07);
        return;
    }
    vfs_create(argv[1]);
    print_string("Created file: ", 0, 1, 0x07);
    print_string(argv[1], 14, 1, 0x07);
}

void cmd_ps(int argc, char *argv[]) {
    print_string("Process List:", 0, 1, 0x07);
    sched_list();
}

void cmd_desktop(int argc, char *argv[]) {
    print_string("Desktop Information:", 0, 1, 0x07);
    desktop_show_info();
}

void cmd_services(int argc, char *argv[]) {
    print_string("System Services:", 0, 1, 0x07);
    timeline_record("shell", "Services status requested");
    // TODO: Show actual services status
}

void cmd_packages(int argc, char *argv[]) {
    print_string("Installed Packages:", 0, 1, 0x07);
    package_t pkg_buf[MAX_PACKAGES];
    uint32_t count = list_packages(pkg_buf, MAX_PACKAGES);
    int y = 2;
    for(uint32_t i = 0; i < count; i++) {
        print_string(pkg_buf[i].name, 0, y, 0x07);
        print_string(pkg_buf[i].version, 20, y, 0x07);
        y++;
    }
}

void cmd_install(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: install <package_name>", 0, 1, 0x07);
        return;
    }
    print_string("Installing ", 0, 1, 0x07);
    print_string(argv[1], 11, 1, 0x07);
    print_string("...", 11 + strlen(argv[1]), 1, 0x07);
    download_package(argv[1]);
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

void cmd_lsnodes(int argc, char *argv[]) {
    print_string("UI Nodes on Canvas:", 0, 1, 0x07);
    desktop_list_nodes();
}

void cmd_connect(int argc, char *argv[]) {
    if (argc < 3) {
        print_string("Usage: connect <node_id_1> <node_id_2>", 0, 1, 0x07);
        return;
    }
    int id1 = atoi(argv[1]);
    int id2 = atoi(argv[2]);
    desktop_connect_nodes(id1, id2);
    print_string("Connected node ", 0, 1, 0x07);
    print_string(argv[1], 16, 1, 0x07);
}

void cmd_alias(int argc, char *argv[]) {
    if (argc < 2) {
        // List all aliases
        print_string("Current Aliases:", 0, 1, 0x07);
        for (int i = 0; i < alias_count; i++) {
            // A real implementation would need a better print_string_at(x,y)
            print_string(aliases[i].name, 0, 2 + i, 0x07);
            print_string(" = \"", 10, 2 + i, 0x07);
            print_string(aliases[i].value, 14, 2 + i, 0x07);
            print_string("\"", 14 + strlen(aliases[i].value), 2 + i, 0x07);
        }
        return;
    }

    // Create a new alias
    if (alias_count < MAX_ALIASES) {
        char* eq = strchr(argv[1], '=');
        if (!eq) {
            print_string("Invalid syntax. Use: alias name=\"command\"", 0, 1, 0x0C);
            return;
        }
        *eq = '\0'; // Split name and value
        strcpy(aliases[alias_count].name, argv[1]);
        strcpy(aliases[alias_count].value, eq + 1); // The value is the rest of the string
        alias_count++;
        print_string("Alias created.", 0, 1, 0x0A);
    }
}

void cmd_do(int argc, char *argv[]) {
    if (argc < 2) {
        print_string("Usage: do \"intent\"", 0, 1, 0x07);
        return;
    }
    char code_buf[256];
    ai_generate_code_from_intent(argv[1], code_buf, sizeof(code_buf));
    print_string(code_buf, 0, 1, 0x07);
}
// Shell main loop
void shell_main() {
    char cmd_buffer[MAX_CMD_LEN];
    int cursor_x = 0, cursor_y = 0;
    int buffer_pos = 0;

    shell_init(); // Show splash
    // Small delay to see splash
    for (volatile int i = 0; i < 50000000; i++);

    clear_screen();
    print_string("AuroraOS > ", 0, 0, 0x07);
    cursor_y = 0;
    cursor_x = 11; // After "AuroraOS > "

    while (true) {
        // Check for keyboard input
        if (keyboard_has_data()) {
            char c = keyboard_getchar();

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

    // --- Self-Improving Language: Check for alias ---
    for (int i = 0; i < alias_count; i++) {
        if (strcmp(cmd_line, aliases[i].name) == 0) {
            print_string("Alias -> ", 0, 1, 0x0B);
            print_string(aliases[i].value, 9, 1, 0x0B);
            // A real implementation would need to move the cursor down
            // and execute the new command. For now, we just show it.
            cmd_line = aliases[i].value;
            break;
        }
    }

    // Parse command and arguments
    char *argv[MAX_ARGS];
    int argc = 0;
    char *saveptr;

    char *token = strtok(cmd_line, " ", &saveptr);
    while (token && argc < MAX_ARGS) {
        argv[argc++] = token;
        token = strtok(NULL, " ", &saveptr);
    }

    if (argc == 0) return;

    // Find and execute command
    for (int i = 0; commands[i].name != NULL; i++) {
        if (strcmp(argv[0], commands[i].name) == 0) {
            ai_log_event(0, cmd_line); // EVENT_COMMAND_SUCCESS
            commands[i].func(argc, argv);
            return;
        }
    }

    // Command not found
    ai_log_event(1, cmd_line); // EVENT_COMMAND_FAIL
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