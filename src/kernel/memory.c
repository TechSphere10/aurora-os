#include "kernel.h"

/* ═══════════════════════════════════════════════════════════════════
   MEMORY  MANAGER
   – Page bitmap allocator  (4 KB pages)
   – Heap  (first-fit, coalescing free)
   ═══════════════════════════════════════════════════════════════════ */

/* ── Page allocator ────────────────────────────────────────────────── */
#define MAX_MEM_MB   256
#define MAX_PAGES    ((MAX_MEM_MB * 1024 * 1024) / PAGE_SIZE)
#define BITMAP_WORDS ((MAX_PAGES + 31) / 32)

static uint32_t page_bitmap[BITMAP_WORDS];
static uint32_t total_pages  = 0;
static uint32_t used_pages   = 0;

static void bitmap_set(uint32_t page) {
    page_bitmap[page / 32] |= (1u << (page % 32));
}
static void bitmap_clear(uint32_t page) {
    page_bitmap[page / 32] &= ~(1u << (page % 32));
}
static bool bitmap_test(uint32_t page) {
    return (page_bitmap[page / 32] >> (page % 32)) & 1;
}

void mem_init(uint32_t mem_upper_kb) {
    kmemset(page_bitmap, 0xFF, sizeof(page_bitmap)); /* all used */
    uint32_t total_kb = mem_upper_kb + 1024; /* lower 1 MB + upper */
    total_pages = (total_kb * 1024) / PAGE_SIZE;
    if (total_pages > MAX_PAGES) total_pages = MAX_PAGES;

    /* Free pages above 1 MB (kernel lives in first 4 MB) */
    uint32_t free_start = (4 * 1024 * 1024) / PAGE_SIZE;
    for (uint32_t i = free_start; i < total_pages; i++) {
        bitmap_clear(i);
    }
    used_pages = free_start;
}

void *page_alloc(void) {
    for (uint32_t i = 0; i < total_pages; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            used_pages++;
            return (void *)(i * PAGE_SIZE);
        }
    }
    return 0;
}

void page_free(void *addr) {
    uint32_t page = (uint32_t)addr / PAGE_SIZE;
    if (page < total_pages && bitmap_test(page)) {
        bitmap_clear(page);
        used_pages--;
    }
}

/* ── Heap allocator ────────────────────────────────────────────────── */
typedef struct block_hdr {
    uint32_t         magic;
    uint32_t         size;      /* payload size */
    bool             free;
    struct block_hdr *next;
    struct block_hdr *prev;
} block_hdr_t;

#define HEAP_MAGIC  0xA0A0A0A0u
static block_hdr_t *heap_head = 0;
static bool heap_ready = 0;

static void heap_init(void) {
    heap_head = (block_hdr_t *)HEAP_START;
    heap_head->magic = HEAP_MAGIC;
    heap_head->size  = HEAP_SIZE - sizeof(block_hdr_t);
    heap_head->free  = true;
    heap_head->next  = 0;
    heap_head->prev  = 0;
    heap_ready = true;
}

void *kmalloc(size_t size) {
    if (!heap_ready) heap_init();
    if (!size) return 0;
    /* Align to 8 bytes */
    size = (size + 7) & ~7u;

    for (block_hdr_t *b = heap_head; b; b = b->next) {
        if (!b->free || b->size < size) continue;
        /* Split if remainder is large enough */
        if (b->size >= size + sizeof(block_hdr_t) + 16) {
            block_hdr_t *nb = (block_hdr_t *)((uint8_t *)b + sizeof(block_hdr_t) + size);
            nb->magic = HEAP_MAGIC;
            nb->size  = b->size - size - sizeof(block_hdr_t);
            nb->free  = true;
            nb->next  = b->next;
            nb->prev  = b;
            if (b->next) b->next->prev = nb;
            b->next = nb;
            b->size = size;
        }
        b->free = false;
        return (uint8_t *)b + sizeof(block_hdr_t);
    }
    return 0; /* OOM */
}

void kfree(void *ptr) {
    if (!ptr) return;
    block_hdr_t *b = (block_hdr_t *)((uint8_t *)ptr - sizeof(block_hdr_t));
    if (b->magic != HEAP_MAGIC) return;
    b->free = true;
    /* Coalesce with next */
    if (b->next && b->next->free) {
        b->size += sizeof(block_hdr_t) + b->next->size;
        b->next  = b->next->next;
        if (b->next) b->next->prev = b;
    }
    /* Coalesce with prev */
    if (b->prev && b->prev->free) {
        b->prev->size += sizeof(block_hdr_t) + b->size;
        b->prev->next  = b->next;
        if (b->next) b->next->prev = b->prev;
    }
}

void mem_stats(uint32_t *total, uint32_t *used, uint32_t *free_bytes) {
    *total = total_pages * PAGE_SIZE;
    *used  = used_pages  * PAGE_SIZE;
    *free_bytes = (total_pages - used_pages) * PAGE_SIZE;
}
