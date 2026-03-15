#include <stdint.h>
#include <stdbool.h>

// Memory management for AuroraOS
#define MEMORY_SIZE 64 * 1024 * 1024  // 64MB
#define PAGE_SIZE 4096
#define MAX_PAGES (MEMORY_SIZE / PAGE_SIZE)

// Memory bitmap for allocation tracking
uint8_t memory_bitmap[MAX_PAGES / 8];
uint32_t total_allocated = 0;

// Initialize memory management
void init_memory() {
    // Clear bitmap
    for (int i = 0; i < sizeof(memory_bitmap); i++) {
        memory_bitmap[i] = 0;
    }

    // Mark kernel area as used (first 1MB)
    for (int i = 0; i < (1024 * 1024) / PAGE_SIZE; i++) {
        set_page_used(i);
    }
}

// Set a page as used
void set_page_used(int page) {
    int byte = page / 8;
    int bit = page % 8;
    memory_bitmap[byte] |= (1 << bit);
}

// Set a page as free
void set_page_free(int page) {
    int byte = page / 8;
    int bit = page % 8;
    memory_bitmap[byte] &= ~(1 << bit);
}

// Check if a page is used
bool is_page_used(int page) {
    int byte = page / 8;
    int bit = page % 8;
    return (memory_bitmap[byte] & (1 << bit)) != 0;
}

// Allocate a page
void* allocate_page() {
    for (int i = 0; i < MAX_PAGES; i++) {
        if (!is_page_used(i)) {
            set_page_used(i);
            total_allocated += PAGE_SIZE;
            return (void*)(i * PAGE_SIZE);
        }
    }
    return NULL; // Out of memory
}

// Free a page
void free_page(void* addr) {
    int page = (uint32_t)addr / PAGE_SIZE;
    if (is_page_used(page)) {
        set_page_free(page);
        total_allocated -= PAGE_SIZE;
    }
}

// Simple heap allocator
#define HEAP_START 0x200000  // 2MB
#define HEAP_SIZE (16 * 1024 * 1024)  // 16MB heap

typedef struct block_header {
    uint32_t size;
    bool free;
    struct block_header* next;
} block_header_t;

block_header_t* heap_start = (block_header_t*)HEAP_START;
bool heap_initialized = false;

// Initialize heap
void init_heap() {
    heap_start->size = HEAP_SIZE - sizeof(block_header_t);
    heap_start->free = true;
    heap_start->next = NULL;
    heap_initialized = true;
}

// Allocate memory from heap
void* malloc(uint32_t size) {
    if (!heap_initialized) {
        init_heap();
    }

    block_header_t* current = heap_start;
    while (current) {
        if (current->free && current->size >= size + sizeof(block_header_t)) {
            // Split block if large enough
            if (current->size > size + sizeof(block_header_t) + sizeof(block_header_t)) {
                block_header_t* new_block = (block_header_t*)((uint32_t)current + sizeof(block_header_t) + size);
                new_block->size = current->size - size - sizeof(block_header_t);
                new_block->free = true;
                new_block->next = current->next;
                current->next = new_block;
                current->size = size;
            }
            current->free = false;
            return (void*)((uint32_t)current + sizeof(block_header_t));
        }
        current = current->next;
    }
    return NULL; // Out of memory
}

// Free memory
void free(void* ptr) {
    if (!ptr) return;

    block_header_t* block = (block_header_t*)((uint32_t)ptr - sizeof(block_header_t));
    block->free = true;

    // Merge with next block if free
    if (block->next && block->next->free) {
        block->size += sizeof(block_header_t) + block->next->size;
        block->next = block->next->next;
    }

    // Merge with previous block (simplified - would need doubly linked list)
}

// Get memory statistics
uint32_t get_total_memory() {
    return MEMORY_SIZE;
}

uint32_t get_used_memory() {
    return total_allocated;
}

uint32_t get_free_memory() {
    return MEMORY_SIZE - total_allocated;
}