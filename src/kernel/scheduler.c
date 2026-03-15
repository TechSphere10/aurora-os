#include <stdint.h>
#include <stdbool.h>

// Process Scheduler for AuroraOS
// Simulates multi-tasking in the OS simulator

#define MAX_PROCESSES 16
#define STACK_SIZE 1024
#define TIME_SLICE_MS 10

typedef enum {
    PROCESS_READY,
    PROCESS_RUNNING,
    PROCESS_BLOCKED,
    PROCESS_TERMINATED
} process_state_t;

typedef struct {
    uint32_t pid;
    char name[32];
    process_state_t state;
    uint32_t priority;
    uint32_t stack[STACK_SIZE];
    uint32_t stack_pointer;
    uint32_t program_counter;
    uint32_t memory_start;
    uint32_t memory_size;
    uint32_t creation_time;
    uint32_t cpu_time;
    uint32_t last_run_time;
} process_t;

process_t processes[MAX_PROCESSES];
uint32_t process_count = 0;
uint32_t current_process = 0;
uint32_t next_pid = 1;
uint32_t system_time = 0;

// Initialize process scheduler
void scheduler_init() {
    // Create idle process (always running)
    create_process("idle", idle_process, 0);
    processes[0].state = PROCESS_RUNNING;
}

// Create a new process
uint32_t create_process(const char *name, void (*entry_point)(), uint32_t priority) {
    if (process_count >= MAX_PROCESSES) return 0;

    uint32_t pid = next_pid++;
    uint32_t index = process_count++;

    strcpy(processes[index].name, name);
    processes[index].pid = pid;
    processes[index].state = PROCESS_READY;
    processes[index].priority = priority;
    processes[index].stack_pointer = STACK_SIZE - 1;
    processes[index].program_counter = (uint32_t)entry_point;
    processes[index].memory_start = 0; // Virtual memory
    processes[index].memory_size = 4096; // 4KB per process
    processes[index].creation_time = system_time;
    processes[index].cpu_time = 0;
    processes[index].last_run_time = system_time;

    return pid;
}

// Terminate a process
void terminate_process(uint32_t pid) {
    for (uint32_t i = 0; i < process_count; i++) {
        if (processes[i].pid == pid) {
            processes[i].state = PROCESS_TERMINATED;
            // Clean up resources
            break;
        }
    }
}

// Schedule next process (round-robin)
void schedule() {
    uint32_t next_process = current_process;

    // Find next ready process
    for (uint32_t i = 1; i < process_count; i++) {
        uint32_t index = (current_process + i) % process_count;
        if (processes[index].state == PROCESS_READY) {
            next_process = index;
            break;
        }
    }

    if (next_process != current_process) {
        // Context switch
        context_switch(current_process, next_process);
        current_process = next_process;
    }

    system_time += TIME_SLICE_MS;
}

// Context switch simulation
void context_switch(uint32_t from_pid, uint32_t to_pid) {
    // Save current process state
    processes[from_pid].cpu_time += system_time - processes[from_pid].last_run_time;
    processes[from_pid].last_run_time = system_time;

    // Restore next process state
    processes[to_pid].last_run_time = system_time;
    processes[to_pid].state = PROCESS_RUNNING;
    processes[from_pid].state = PROCESS_READY;
}

// Get process list
uint32_t get_process_list(process_t *buffer, uint32_t max_count) {
    uint32_t count = 0;
    for (uint32_t i = 0; i < process_count && count < max_count; i++) {
        if (processes[i].state != PROCESS_TERMINATED) {
            buffer[count++] = processes[i];
        }
    }
    return count;
}

// Get current process
process_t* get_current_process() {
    return &processes[current_process];
}

// Block current process
void block_current_process() {
    processes[current_process].state = PROCESS_BLOCKED;
    schedule();
}

// Wake up process
void wakeup_process(uint32_t pid) {
    for (uint32_t i = 0; i < process_count; i++) {
        if (processes[i].pid == pid && processes[i].state == PROCESS_BLOCKED) {
            processes[i].state = PROCESS_READY;
            break;
        }
    }
}

// Idle process (always running when no other processes)
void idle_process() {
    while (true) {
        // Do nothing - just wait
        __asm__ volatile("nop");
    }
}

// Sleep for milliseconds
void sleep_ms(uint32_t ms) {
    uint32_t wake_time = system_time + ms;
    processes[current_process].state = PROCESS_BLOCKED;

    // In real implementation, this would be handled by timer interrupt
    while (system_time < wake_time) {
        schedule();
    }

    processes[current_process].state = PROCESS_READY;
}

// Yield CPU to other processes
void yield() {
    schedule();
}

// Get system uptime
uint32_t get_system_uptime() {
    return system_time;
}

// Get CPU usage statistics
void get_cpu_stats(uint32_t *total_time, uint32_t *idle_time) {
    *total_time = system_time;
    *idle_time = processes[0].cpu_time; // Idle process CPU time
}

// String functions
char *strcpy(char *dest, const char *src) {
    char *d = dest;
    while ((*d++ = *src++));
    return dest;
}