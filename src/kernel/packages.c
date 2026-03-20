#include <stdint.h>
#include <stdbool.h>
#include "../auroralang/string.h"
#include "kernel.h"

// Log levels for package manager
#define LOG_INFO 0
#define LOG_WARNING 1
#define LOG_ERROR 2

// Package Management System for AuroraOS
// Simulates software installation and management

#define MAX_PACKAGES 20
#define MAX_PACKAGE_NAME 32
#define MAX_PACKAGE_VERSION 16
#define MAX_DEPENDENCIES 5

typedef enum {
    PACKAGE_INSTALLED,
    PACKAGE_AVAILABLE,
    PACKAGE_UPDATING,
    PACKAGE_REMOVING
} package_state_t;

typedef struct {
    char name[MAX_PACKAGE_NAME];
    char version[MAX_PACKAGE_VERSION];
    char description[128];
    package_state_t state;
    uint32_t size_kb;
    uint32_t install_time;
    char dependencies[MAX_DEPENDENCIES][MAX_PACKAGE_NAME];
    uint32_t dep_count;
    bool system_package;
} package_t;

package_t packages[MAX_PACKAGES];
uint32_t package_count = 0;

// Available packages in repository
const char* available_packages[] = {
    "calculator", "text-editor", "file-manager", "terminal",
    "web-browser", "media-player", "image-viewer", "system-monitor",
    "auroralang-ide", "game-engine", "network-tools", "development-kit"
};

const char* package_descriptions[] = {
    "Basic calculator application",
    "Simple text editor",
    "File system browser",
    "Enhanced terminal emulator",
    "Web browser (simulated)",
    "Media player for audio/video",
    "Image viewing application",
    "System resource monitor",
    "AuroraLang development environment",
    "Simple 2D game engine",
    "Network utilities and tools",
    "Complete development toolkit"
};

// Forward declarations
bool install_package(const char *name, const char *version, bool system_package);
void log_message(int level, const char *message);

// Initialize package system
void packages_init() {
    // Install system packages
    install_package("kernel", "1.0", true);
    install_package("shell", "1.0", true);
    install_package("auroralang", "1.0", true);
    install_package("desktop", "1.0", true);
}

// Install a package
bool install_package(const char *name, const char *version, bool system_package) {
    if (package_count >= MAX_PACKAGES) return false;

    // Check if already installed
    for (uint32_t i = 0; i < package_count; i++) {
        if (strcmp(packages[i].name, name) == 0) {
            return false; // Already installed
        }
    }

    uint32_t index = package_count++;
    strcpy(packages[index].name, name);
    strcpy(packages[index].version, version);
    packages[index].state = PACKAGE_INSTALLED;
    packages[index].size_kb = 1024 + (rand() % 4096); // Random size
    packages[index].install_time = get_system_time();
    packages[index].dep_count = 0;
    packages[index].system_package = system_package;

    // Set description
    for (uint32_t i = 0; i < sizeof(available_packages)/sizeof(available_packages[0]); i++) {
        if (strcmp(available_packages[i], name) == 0) {
            strcpy(packages[index].description, package_descriptions[i]);
            break;
        }
    }

    // Add to file system
    char path[64];
    ksnprintf(path, sizeof(path), "/apps/%s", name);
    vfs_mkdir(path);

    log_message(LOG_INFO, "Installed package: ");
    log_message(LOG_INFO, name);

    return true;
}

// Remove a package
bool remove_package(const char *name) {
    for (uint32_t i = 0; i < package_count; i++) {
        if (strcmp(packages[i].name, name) == 0) {
            if (packages[i].system_package) {
                return false; // Cannot remove system packages
            }

            packages[i].state = PACKAGE_REMOVING;

            // Remove from file system
            char path[64];
            ksnprintf(path, sizeof(path), "/apps/%s", name);
            vfs_delete(path);

            // Mark as removed
            packages[i].name[0] = '\0';

            log_message(LOG_INFO, "Removed package: ");
            log_message(LOG_INFO, name);
            return true;
        }
    }
    return false;
}

// Update a package
bool update_package(const char *name) {
    for (uint32_t i = 0; i < package_count; i++) {
        if (strcmp(packages[i].name, name) == 0) {
            packages[i].state = PACKAGE_UPDATING;

            // Simulate version increment
            char *ver = packages[i].version;
            if (ver[1] < '9') {
                ver[1]++;
            } else {
                ver[0]++;
                ver[1] = '0';
            }

            packages[i].state = PACKAGE_INSTALLED;

            log_message(LOG_INFO, "Updated package: ");
            log_message(LOG_INFO, name);
            return true;
        }
    }
    return false;
}

// List installed packages
uint32_t list_packages(package_t *buffer, uint32_t max_count) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < package_count && count < max_count; i++) {
        if (packages[i].name[0] != '\0') {
            buffer[count++] = packages[i];
        }
    }
    return count;
}

// List available packages
uint32_t list_available_packages(char *buffer, uint32_t buffer_size) {
    uint32_t count = 0;
    uint32_t offset = 0;

    for (uint32_t i = 0; i < sizeof(available_packages)/sizeof(available_packages[0]); i++) {
        const char *name = available_packages[i];
        bool installed = false;

        // Check if already installed
        for (uint32_t j = 0; j < package_count; j++) {
            if (strcmp(packages[j].name, name) == 0) {
                installed = true;
                break;
            }
        }

        if (!installed && offset < buffer_size - 64) {
            offset += ksnprintf(buffer + offset, buffer_size - offset, "%s - %s\n",
                            name, package_descriptions[i]);
            count++;
        }
    }

    return count;
}

// Check package dependencies
bool check_dependencies(const char *package_name) {
    // Simplified - assume all dependencies are met
    return true;
}

// Get package info
bool get_package_info(const char *name, package_t *info) {
    for (uint32_t i = 0; i < package_count; i++) {
        if (strcmp(packages[i].name, name) == 0) {
            *info = packages[i];
            return true;
        }
    }
    return false;
}

// Package repository operations
bool download_package(const char *name) {
    // Simulate download
    for (uint32_t i = 0; i < sizeof(available_packages)/sizeof(available_packages[0]); i++) {
        if (strcmp(available_packages[i], name) == 0) {
            return install_package(name, "1.0", false);
        }
    }
    return false;
}

// Update all packages
void update_all_packages() {
    for (uint32_t i = 0; i < package_count; i++) {
        if (packages[i].name[0] != '\0' && !packages[i].system_package) {
            update_package(packages[i].name);
        }
    }
}

// Get total installed packages size
uint32_t get_installed_size() {
    uint32_t total = 0;
    for (uint32_t i = 0; i < package_count; i++) {
        if (packages[i].name[0] != '\0') {
            total += packages[i].size_kb;
        }
    }
    return total;
}

// Helper functions implementation
uint32_t get_system_time() {
    return timer_seconds();
}

void log_message(int level, const char *message) {
    // Map to kernel timeline
    const char *cat = (level == LOG_ERROR) ? "error" : "package";
    timeline_record(cat, message);
}

int rand() {
    static uint32_t seed = 12345;
    seed = seed * 1103515245 + 12345;
    return (seed >> 16) & 0x7FFF;
}