#include "kernel.h"

/* ═══════════════════════════════════════════════════════════════════
   PS/2  KEYBOARD  DRIVER
   ═══════════════════════════════════════════════════════════════════ */

#define KB_DATA   0x60
#define KB_STATUS 0x64
#define KB_BUF    256

static char kb_buf[KB_BUF];
static volatile int kb_head = 0, kb_tail = 0;
static bool kb_shift = false, kb_ctrl = false, kb_caps = false;

/* US QWERTY scancode set 1 – normal and shifted */
static const char sc_normal[128] = {
    0,  27, '1','2','3','4','5','6','7','8','9','0','-','=','\b',
    '\t','q','w','e','r','t','y','u','i','o','p','[',']','\n',
    0,  'a','s','d','f','g','h','j','k','l',';','\'','`',
    0,  '\\','z','x','c','v','b','n','m',',','.','/', 0,
    '*', 0, ' ', 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  '-', 0,  0,  0,  '+', 0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static const char sc_shift[128] = {
    0,  27, '!','@','#','$','%','^','&','*','(',')','_','+','\b',
    '\t','Q','W','E','R','T','Y','U','I','O','P','{','}','\n',
    0,  'A','S','D','F','G','H','J','K','L',':','"','~',
    0,  '|','Z','X','C','V','B','N','M','<','>','?', 0,
    '*', 0, ' ', 0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  '-', 0,  0,  0,  '+', 0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0
};

static void kb_irq_handler(void) {
    uint8_t sc = inb(KB_DATA);
    bool released = (sc & 0x80) != 0;
    uint8_t key   = sc & 0x7F;

    /* Modifier keys */
    if (key == 0x2A || key == 0x36) { kb_shift = !released; return; }
    if (key == 0x1D)                { kb_ctrl  = !released; return; }
    if (key == 0x3A && !released)   { kb_caps  = !kb_caps;  return; }
    if (released) return;

    char c = 0;
    bool upper = kb_shift ^ kb_caps;
    if (key < 128) c = upper ? sc_shift[key] : sc_normal[key];
    if (!c) return;

    /* Ctrl combos */
    if (kb_ctrl) {
        if (c >= 'a' && c <= 'z') c = (char)(c - 'a' + 1); /* Ctrl+A=1 */
        else if (c >= 'A' && c <= 'Z') c = (char)(c - 'A' + 1);
    }

    int next = (kb_head + 1) % KB_BUF;
    if (next != kb_tail) {
        kb_buf[kb_head] = c;
        kb_head = next;
    }
}

void keyboard_init(void) {
    irq_install_handler(1, kb_irq_handler);
    /* Unmask IRQ1 */
    outb(0x21, inb(0x21) & ~0x02);
}

bool keyboard_has_data(void) { return kb_head != kb_tail; }

char keyboard_getchar(void) {
    while (!keyboard_has_data()) __asm__ volatile("hlt");
    char c = kb_buf[kb_tail];
    kb_tail = (kb_tail + 1) % KB_BUF;
    return c;
}
