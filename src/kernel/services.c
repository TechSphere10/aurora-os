#include <stdint.h>
#include <stdbool.h>

// System Services for AuroraOS
// Provides system monitoring, logging, and configuration

#define LOG_BUFFER_SIZE 1024
#define CONFIG_MAX_ENTRIES 32
#define SERVICE_MAX_COUNT 8

typedef enum {
    LOG_INFO,
    LOG_WARNING,
    LOG_ERROR,
    LOG_DEBUG
} log_level_t;

typedef struct {
    uint32_t timestamp;
    log_level_t level;
    char message[128];
} log_entry_t;

typedef struct {
    char key[32];
    char value[64];
} config_entry_t;

typedef struct {
    char name[32];
    bool running;
    uint32_t start_time;
    void (*service_function)();
} system_service_t;

// Global system state
log_entry_t log_buffer[LOG_BUFFER_SIZE];
uint32_t log_count = 0;
uint32_t log_head = 0;

config_entry_t config[CONFIG_MAX_ENTRIES];
uint32_t config_count = 0;

system_service_t services[SERVICE_MAX_COUNT];
uint32_t service_count = 0;

// System monitoring
uint32_t system_start_time = 0;
uint32_t interrupt_count = 0;
uint32_t system_calls_count = 0;

// Initialize system services
void services_init() {
    system_start_time = get_system_time();

    // Load default configuration
    set_config("system.name", "AuroraOS");
    set_config("system.version", "1.0");
    set_config("desktop.enabled", "true");
    set_config("logging.level", "info");
    set_config("memory.limit", "64MB");

    // Start essential services
    register_service("logger", logger_service);
    register_service("monitor", monitor_service);
    register_service("scheduler", scheduler_service);

    start_service("logger");
    start_service("monitor");
}

// Logging system
void log_message(log_level_t level, const char *message) {
    if (log_count < LOG_BUFFER_SIZE) {
        log_entry_t *entry = &log_buffer[log_head];
        entry->timestamp = get_system_time();
        entry->level = level;
        strcpy(entry->message, message);

        log_head = (log_head + 1) % LOG_BUFFER_SIZE;
        if (log_count < LOG_BUFFER_SIZE) log_count++;
    }
}

void get_logs(char *buffer, uint32_t buffer_size, uint32_t *count) {
    *count = 0;
    uint32_t offset = 0;

    for (uint32_t i = 0; i < log_count && offset < buffer_size - 64; i++) {
        uint32_t index = (log_head - 1 - i + LOG_BUFFER_SIZE) % LOG_BUFFER_SIZE;
        log_entry_t *entry = &log_buffer[index];

        const char *level_str = "UNK";
        switch (entry->level) {
            case LOG_INFO: level_str = "INFO"; break;
            case LOG_WARNING: level_str = "WARN"; break;
            case LOG_ERROR: level_str = "ERROR"; break;
            case LOG_DEBUG: level_str = "DEBUG"; break;
        }

        offset += sprintf(buffer + offset, "[%s] %s\n", level_str, entry->message);
        (*count)++;
    }
}

// Configuration management
void set_config(const char *key, const char *value) {
    for (uint32_t i = 0; i < config_count; i++) {
        if (strcmp(config[i].key, key) == 0) {
            strcpy(config[i].value, value);
            return;
        }
    }

    if (config_count < CONFIG_MAX_ENTRIES) {
        strcpy(config[config_count].key, key);
        strcpy(config[config_count].value, value);
        config_count++;
    }
}

const char* get_config(const char *key) {
    for (uint32_t i = 0; i < config_count; i++) {
        if (strcmp(config[i].key, key) == 0) {
            return config[i].value;
        }
    }
    return NULL;
}

// Service management
void register_service(const char *name, void (*service_func)()) {
    if (service_count >= SERVICE_MAX_COUNT) return;

    strcpy(services[service_count].name, name);
    services[service_count].running = false;
    services[service_count].start_time = 0;
    services[service_count].service_function = service_func;
    service_count++;
}

void start_service(const char *name) {
    for (uint32_t i = 0; i < service_count; i++) {
        if (strcmp(services[i].name, name) == 0 && !services[i].running) {
            services[i].running = true;
            services[i].start_time = get_system_time();
            log_message(LOG_INFO, "Started service: ");
            log_message(LOG_INFO, name);
            break;
        }
    }
}

void stop_service(const char *name) {
    for (uint32_t i = 0; i < service_count; i++) {
        if (strcmp(services[i].name, name) == 0 && services[i].running) {
            services[i].running = false;
            log_message(LOG_INFO, "Stopped service: ");
            log_message(LOG_INFO, name);
            break;
        }
    }
}

// System monitoring
void get_system_stats(uint32_t *uptime, uint32_t *interrupts, uint32_t *syscalls) {
    *uptime = get_system_time() - system_start_time;
    *interrupts = interrupt_count;
    *syscalls = system_calls_count;
}

void increment_interrupt_count() {
    interrupt_count++;
}

void increment_syscall_count() {
    system_calls_count++;
}

// Service functions
void logger_service() {
    // Logger service - runs periodically
    static uint32_t last_log = 0;
    if (get_system_time() - last_log > 1000) { // Every second
        log_message(LOG_INFO, "System running normally");
        last_log = get_system_time();
    }
}

void monitor_service() {
    // Monitor system resources
    static uint32_t last_check = 0;
    if (get_system_time() - last_check > 5000) { // Every 5 seconds
        uint32_t used, total;
        get_memory_stats(&used, &total);
        if (used > total * 80 / 100) {
            log_message(LOG_WARNING, "High memory usage detected");
        }
        last_check = get_system_time();
    }
}

void scheduler_service() {
    // Handle process scheduling
    schedule_processes();
}

// Forward declarations
uint32_t get_system_time();
void get_memory_stats(uint32_t *used, uint32_t *total);
void schedule_processes();

// String functions
int strcmp(const char *s1, const char *s2) {
    while (*s1 && *s2 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}

int sprintf(char *buffer, const char *format, ...) {
    // Simple sprintf implementation
    strcpy(buffer, format);
    return strlen(buffer);
}

size_t strlen(const char *s) {
    size_t len = 0;
    while (s[len]) len++;
    return len;
}