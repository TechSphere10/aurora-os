#include <stdint.h>
#include <stdbool.h>

// Keyboard driver for AuroraOS
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Scancode to ASCII mapping (simplified)
char scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0, ' ', 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, '-', 0, 0, 0, '+', 0, 0, 0, 0, 0, 0, 0
};

bool shift_pressed = false;
bool caps_lock = false;

// Keyboard buffer
#define KEYBOARD_BUFFER_SIZE 256
char keyboard_buffer[KEYBOARD_BUFFER_SIZE];
int buffer_head = 0;
int buffer_tail = 0;

// Initialize keyboard
void init_keyboard() {
    // Enable keyboard interrupts (simplified)
    // In real implementation, would set up IDT and PIC
}

// Read keyboard scancode
uint8_t read_keyboard_scancode() {
    while ((inb(KEYBOARD_STATUS_PORT) & 1) == 0);
    return inb(KEYBOARD_DATA_PORT);
}

// Convert scancode to ASCII
char scancode_to_char(uint8_t scancode) {
    if (scancode & 0x80) {
        // Key release
        if (scancode == 0xAA || scancode == 0xB6) {
            shift_pressed = false;
        }
        return 0;
    }

    // Key press
    if (scancode == 0x2A || scancode == 0x36) {
        shift_pressed = true;
        return 0;
    }

    if (scancode == 0x3A) {
        caps_lock = !caps_lock;
        return 0;
    }

    char c = scancode_to_ascii[scancode];
    if (c >= 'a' && c <= 'z') {
        if (shift_pressed || caps_lock) {
            c -= 32; // Convert to uppercase
        }
    } else if (shift_pressed) {
        // Handle shift for other characters
        switch (c) {
            case '1': c = '!'; break;
            case '2': c = '@'; break;
            case '3': c = '#'; break;
            case '4': c = '$'; break;
            case '5': c = '%'; break;
            case '6': c = '^'; break;
            case '7': c = '&'; break;
            case '8': c = '*'; break;
            case '9': c = '('; break;
            case '0': c = ')'; break;
            case '-': c = '_'; break;
            case '=': c = '+'; break;
            case '[': c = '{'; break;
            case ']': c = '}'; break;
            case '\\': c = '|'; break;
            case ';': c = ':'; break;
            case '\'': c = '"'; break;
            case ',': c = '<'; break;
            case '.': c = '>'; break;
            case '/': c = '?'; break;
        }
    }

    return c;
}

// Add character to keyboard buffer
void keyboard_buffer_put(char c) {
    if (c == 0) return;

    int next = (buffer_head + 1) % KEYBOARD_BUFFER_SIZE;
    if (next != buffer_tail) {
        keyboard_buffer[buffer_head] = c;
        buffer_head = next;
    }
}

// Get character from keyboard buffer
char keyboard_buffer_get() {
    if (buffer_head == buffer_tail) {
        return 0;
    }

    char c = keyboard_buffer[buffer_tail];
    buffer_tail = (buffer_tail + 1) % KEYBOARD_BUFFER_SIZE;
    return c;
}

// Check if keyboard buffer has data
bool keyboard_has_data() {
    return buffer_head != buffer_tail;
}

// Keyboard interrupt handler (simplified)
void keyboard_interrupt_handler() {
    uint8_t scancode = read_keyboard_scancode();
    char c = scancode_to_char(scancode);
    if (c) {
        keyboard_buffer_put(c);
    }
}

// Port I/O functions
uint8_t inb(uint16_t port) {
    uint8_t result;
    __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(port));
    return result;
}

void outb(uint16_t port, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}