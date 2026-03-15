#include <stdint.h>
#include <stdbool.h>

// Simple Desktop GUI for AuroraOS
// Text-based GUI simulation for the OS

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25
#define DESKTOP_COLOR 0x1F
#define WINDOW_COLOR 0x70
#define TITLEBAR_COLOR 0x2F

typedef struct {
    char title[32];
    int x, y, width, height;
    bool visible;
    bool focused;
    uint32_t background_color;
    char content[1024];
} window_t;

#define MAX_WINDOWS 8
window_t windows[MAX_WINDOWS];
int window_count = 0;
int focused_window = -1;

// Desktop state
bool desktop_mode = false;
char desktop_icons[10][32];
int icon_count = 0;

// Initialize desktop
void desktop_init() {
    desktop_mode = true;

    // Add some default icons
    strcpy(desktop_icons[0], "Terminal");
    strcpy(desktop_icons[1], "File Manager");
    strcpy(desktop_icons[2], "Calculator");
    strcpy(desktop_icons[3], "AuroraLang IDE");
    strcpy(desktop_icons[4], "System Monitor");
    icon_count = 5;
}

// Create a window
int create_window(const char *title, int x, int y, int width, int height) {
    if (window_count >= MAX_WINDOWS) return -1;

    int index = window_count++;
    strcpy(windows[index].title, title);
    windows[index].x = x;
    windows[index].y = y;
    windows[index].width = width;
    windows[index].height = height;
    windows[index].visible = true;
    windows[index].focused = false;
    windows[index].background_color = WINDOW_COLOR;
    windows[index].content[0] = '\0';

    // Focus the new window
    set_focused_window(index);

    return index;
}

// Set focused window
void set_focused_window(int index) {
    if (focused_window >= 0) {
        windows[focused_window].focused = false;
    }
    focused_window = index;
    if (index >= 0) {
        windows[index].focused = true;
    }
}

// Draw desktop
void draw_desktop() {
    if (!desktop_mode) return;

    // Clear screen with desktop color
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            print_char(' ', x, y, DESKTOP_COLOR);
        }
    }

    // Draw desktop title
    print_string("AuroraOS Desktop", 1, 0, 0x1F);
    print_string("Press ESC for shell", 50, 0, 0x1F);

    // Draw icons
    for (int i = 0; i < icon_count; i++) {
        int x = 2 + (i % 5) * 15;
        int y = 3 + (i / 5) * 4;

        // Icon box
        for (int dy = 0; dy < 3; dy++) {
            for (int dx = 0; dx < 12; dx++) {
                print_char(' ', x + dx, y + dy, 0x70);
            }
        }

        // Icon text
        print_string(desktop_icons[i], x + 1, y + 1, 0x7F);
    }

    // Draw windows
    for (int i = 0; i < window_count; i++) {
        if (windows[i].visible) {
            draw_window(&windows[i]);
        }
    }
}

// Draw a window
void draw_window(window_t *window) {
    uint32_t color = window->background_color;
    if (window->focused) {
        color = (color & 0xF0) | 0x0F; // Brighter for focused
    }

    // Window border
    for (int y = window->y; y < window->y + window->height; y++) {
        for (int x = window->x; x < window->x + window->width; x++) {
            if (y == window->y || y == window->y + window->height - 1 ||
                x == window->x || x == window->x + window->width - 1) {
                print_char(' ', x, y, TITLEBAR_COLOR);
            } else {
                print_char(' ', x, y, color);
            }
        }
    }

    // Title bar
    print_string(window->title, window->x + 2, window->y, TITLEBAR_COLOR);

    // Close button
    print_char('X', window->x + window->width - 2, window->y, 0x4F);

    // Window content
    int content_y = window->y + 1;
    char *line = window->content;
    while (*line && content_y < window->y + window->height - 1) {
        char *end = line;
        while (*end && *end != '\n' && (end - line) < window->width - 2) end++;

        for (char *c = line; c < end; c++) {
            print_char(*c, window->x + 1 + (c - line), content_y, color);
        }

        if (*end == '\n') {
            line = end + 1;
        } else {
            line = end;
        }
        content_y++;
    }
}

// Handle desktop input
void handle_desktop_input(char key) {
    if (key == 27) { // ESC
        desktop_mode = false;
        return;
    }

    // Handle window focus and clicks
    // Simplified - in real implementation would handle mouse
}

// Set window content
void set_window_content(int window_id, const char *content) {
    if (window_id >= 0 && window_id < window_count) {
        strcpy(windows[window_id].content, content);
    }
}

// Close window
void close_window(int window_id) {
    if (window_id >= 0 && window_id < window_count) {
        windows[window_id].visible = false;
        if (focused_window == window_id) {
            focused_window = -1;
        }
    }
}

// Get desktop mode
bool is_desktop_mode() {
    return desktop_mode;
}

// Screen functions (forward declarations)
void print_char(char c, int x, int y, uint32_t color);
void print_string(const char *str, int x, int y, uint32_t color);

// String functions
char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}