#include <stdint.h>
#include <stdbool.h>

// Virtual File System for AuroraOS
// Simulates a file system in memory for the OS simulator

#define MAX_FILES 100
#define MAX_FILENAME_LEN 32
#define MAX_FILE_SIZE 4096
#define MAX_PATH_LEN 128

typedef struct {
    char name[MAX_FILENAME_LEN];
    uint32_t size;
    uint8_t data[MAX_FILE_SIZE];
    bool is_directory;
    uint32_t parent_index;
    uint32_t created_time;
    uint32_t modified_time;
} vfs_node_t;

vfs_node_t vfs_nodes[MAX_FILES];
uint32_t next_node_index = 0;
uint32_t root_directory = 0;

// Initialize virtual file system
void vfs_init() {
    // Create root directory
    strcpy(vfs_nodes[0].name, "/");
    vfs_nodes[0].size = 0;
    vfs_nodes[0].is_directory = true;
    vfs_nodes[0].parent_index = 0;
    vfs_nodes[0].created_time = 0;
    vfs_nodes[0].modified_time = 0;
    next_node_index = 1;

    // Create basic directories
    vfs_create_directory("/home");
    vfs_create_directory("/system");
    vfs_create_directory("/apps");
    vfs_create_directory("/config");
    vfs_create_directory("/temp");
}

// Create a directory
uint32_t vfs_create_directory(const char *path) {
    if (next_node_index >= MAX_FILES) return 0;

    char dirname[MAX_FILENAME_LEN];
    uint32_t parent = vfs_get_parent_directory(path, dirname);

    if (parent == (uint32_t)-1) return 0;

    // Check if directory already exists
    for (uint32_t i = 0; i < next_node_index; i++) {
        if (vfs_nodes[i].parent_index == parent &&
            strcmp(vfs_nodes[i].name, dirname) == 0) {
            return 0; // Already exists
        }
    }

    uint32_t index = next_node_index++;
    strcpy(vfs_nodes[index].name, dirname);
    vfs_nodes[index].size = 0;
    vfs_nodes[index].is_directory = true;
    vfs_nodes[index].parent_index = parent;
    vfs_nodes[index].created_time = get_system_time();
    vfs_nodes[index].modified_time = get_system_time();

    return index;
}

// Create a file
uint32_t vfs_create_file(const char *path) {
    if (next_node_index >= MAX_FILES) return 0;

    char filename[MAX_FILENAME_LEN];
    uint32_t parent = vfs_get_parent_directory(path, filename);

    if (parent == (uint32_t)-1) return 0;

    uint32_t index = next_node_index++;
    strcpy(vfs_nodes[index].name, filename);
    vfs_nodes[index].size = 0;
    vfs_nodes[index].is_directory = false;
    vfs_nodes[index].parent_index = parent;
    vfs_nodes[index].created_time = get_system_time();
    vfs_nodes[index].modified_time = get_system_time();

    return index;
}

// Write to file
bool vfs_write_file(const char *path, const uint8_t *data, uint32_t size) {
    uint32_t file_index = vfs_find_file(path);
    if (file_index == 0 || vfs_nodes[file_index].is_directory) return false;

    if (size > MAX_FILE_SIZE) size = MAX_FILE_SIZE;

    memcpy(vfs_nodes[file_index].data, data, size);
    vfs_nodes[file_index].size = size;
    vfs_nodes[file_index].modified_time = get_system_time();

    return true;
}

// Read from file
bool vfs_read_file(const char *path, uint8_t *buffer, uint32_t *size) {
    uint32_t file_index = vfs_find_file(path);
    if (file_index == 0 || vfs_nodes[file_index].is_directory) return false;

    uint32_t read_size = vfs_nodes[file_index].size;
    if (*size < read_size) read_size = *size;

    memcpy(buffer, vfs_nodes[file_index].data, read_size);
    *size = read_size;

    return true;
}

// Find file by path
uint32_t vfs_find_file(const char *path) {
    if (strcmp(path, "/") == 0) return 0;

    char path_copy[MAX_PATH_LEN];
    strcpy(path_copy, path);

    char *token = strtok(path_copy, "/");
    uint32_t current = 0;

    while (token) {
        bool found = false;
        for (uint32_t i = 0; i < next_node_index; i++) {
            if (vfs_nodes[i].parent_index == current &&
                strcmp(vfs_nodes[i].name, token) == 0) {
                current = i;
                found = true;
                break;
            }
        }
        if (!found) return 0;
        token = strtok(NULL, "/");
    }

    return current;
}

// List directory contents
uint32_t vfs_list_directory(const char *path, char *buffer, uint32_t buffer_size) {
    uint32_t dir_index = vfs_find_file(path);
    if (dir_index == 0 || !vfs_nodes[dir_index].is_directory) return 0;

    uint32_t count = 0;
    uint32_t offset = 0;

    for (uint32_t i = 0; i < next_node_index; i++) {
        if (vfs_nodes[i].parent_index == dir_index) {
            if (offset + MAX_FILENAME_LEN + 2 < buffer_size) {
                if (vfs_nodes[i].is_directory) {
                    buffer[offset++] = '[';
                    strcpy(buffer + offset, vfs_nodes[i].name);
                    offset += strlen(vfs_nodes[i].name);
                    buffer[offset++] = ']';
                } else {
                    strcpy(buffer + offset, vfs_nodes[i].name);
                    offset += strlen(vfs_nodes[i].name);
                }
                buffer[offset++] = '\n';
                count++;
            }
        }
    }

    buffer[offset] = '\0';
    return count;
}

// Delete file or directory
bool vfs_delete(const char *path) {
    uint32_t index = vfs_find_file(path);
    if (index == 0) return false;

    // Mark as deleted (simple implementation)
    vfs_nodes[index].name[0] = '\0';
    return true;
}

// Get file info
bool vfs_get_info(const char *path, uint32_t *size, bool *is_dir, uint32_t *created, uint32_t *modified) {
    uint32_t index = vfs_find_file(path);
    if (index == 0) return false;

    *size = vfs_nodes[index].size;
    *is_dir = vfs_nodes[index].is_directory;
    *created = vfs_nodes[index].created_time;
    *modified = vfs_nodes[index].modified_time;
    return true;
}

// Helper functions
uint32_t vfs_get_parent_directory(const char *path, char *basename) {
    char path_copy[MAX_PATH_LEN];
    strcpy(path_copy, path);

    char *last_slash = strrchr(path_copy, '/');
    if (!last_slash) return (uint32_t)-1;

    *last_slash = '\0';
    strcpy(basename, last_slash + 1);

    if (strlen(path_copy) == 0) return 0; // Root

    return vfs_find_file(path_copy);
}

uint32_t get_system_time() {
    // Simple time simulation
    static uint32_t time = 0;
    return time++;
}

// String functions
char *strtok(char *str, const char *delim) {
    static char *last = NULL;
    if (str) last = str;
    if (!last) return NULL;

    char *start = last;
    while (*last && !strchr(delim, *last)) last++;
    if (*last) *last++ = '\0';
    return start;
}

char *strrchr(const char *s, int c) {
    const char *last = NULL;
    while (*s) {
        if (*s == c) last = s;
        s++;
    }
    return (char*)last;
}

char *strchr(const char *s, int c) {
    while (*s) {
        if (*s == c) return (char*)s;
        s++;
    }
    return NULL;
}

int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

void *memcpy(void *dest, const void *src, size_t n) {
    char *d = dest;
    const char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}