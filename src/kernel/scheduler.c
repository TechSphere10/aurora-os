#include "kernel.h"

/* ═══════════════════════════════════════════════════════════════════
   PROCESS  SCHEDULER  –  Priority Round-Robin
   ═══════════════════════════════════════════════════════════════════ */

static process_t procs[MAX_PROCS];
static int       proc_count   = 0;
static int       current_proc = 0;
static uint32_t  next_pid     = 1;

void sched_init(void) {
    kmemset(procs, 0, sizeof(procs));
    /* PID 0 = kernel idle */
    procs[0].pid        = 0;
    kstrcpy(procs[0].name, "idle");
    procs[0].state      = PROC_RUNNING;
    procs[0].host_pid   = 0; // Idle has no host
    procs[0].priority   = 0;
    procs[0].created_at = timer_seconds();
    procs[0].memory_kb  = 64;
    proc_count = 1;
}

uint32_t sched_spawn(const char *name, uint32_t priority, uint32_t host_pid) {
    if (proc_count >= MAX_PROCS) return 0;
    int idx = proc_count++;
    procs[idx].pid        = next_pid++;
    kstrncpy(procs[idx].name, name, PROC_NAME_LEN - 1);
    procs[idx].state      = PROC_READY;
    procs[idx].priority   = priority;
    procs[idx].created_at = timer_seconds();
    procs[idx].cpu_time   = 0;
    procs[idx].host_pid   = host_pid;
    procs[idx].memory_kb  = 128 + priority * 32;
    char buf[64];
    if (host_pid != 0) {
        ksnprintf(buf, sizeof(buf), "Symbiote spawned: %s (pid=%u, host=%u)", name, procs[idx].pid, host_pid);
    } else {
        ksnprintf(buf, sizeof(buf), "Process spawned: %s (pid=%u)", name, procs[idx].pid);
    }
    timeline_record("kernel", buf);
    return procs[idx].pid;
}

void sched_kill(uint32_t pid) {
    if (pid == 0) return; // Cannot kill idle process
    for (int i = 1; i < proc_count; i++) {
        if (procs[i].pid == pid) {
            procs[i].state = PROC_DEAD;
            char buf[64];
            ksnprintf(buf, sizeof(buf), "Process killed: %s (pid=%u)", procs[i].name, pid);
            timeline_record("kernel", buf);

            // Recursively kill all symbiotes of this process
            for (int j = 1; j < proc_count; j++) {
                if (procs[j].host_pid == pid) {
                    sched_kill(procs[j].pid);
                }
            }
            return;
        }
    }
}

void sched_tick(void) {
    /* Increment CPU time for current process */
    if (current_proc < proc_count)
        procs[current_proc].cpu_time++;
    /* Round-robin: find next READY process */
    for (int i = 1; i <= proc_count; i++) {
        int next = (current_proc + i) % proc_count;
        if (procs[next].state == PROC_READY || procs[next].state == PROC_RUNNING) {
            if (current_proc != next) {
                if (procs[current_proc].state == PROC_RUNNING)
                    procs[current_proc].state = PROC_READY;
                procs[next].state = PROC_RUNNING;
                current_proc = next;
            }
            return;
        }
    }
}

void sched_list(void) {
    term_setcolor(VGA_COLOR(VGA_LIGHT_CYAN, VGA_BLACK));
    term_writeln("  PID   HOST  NAME              STATE     PRI  CPU(t)  MEM(KB)  ACTIVITY");
    term_writeln("  ───────────────────────────────────────────────────────────────────────");
    term_setcolor(VGA_COLOR(VGA_LIGHT_GREY, VGA_BLACK));
    for (int i = 0; i < proc_count; i++) {
        if (procs[i].state == PROC_DEAD) continue;
        const char *st = "READY  ";
        if (procs[i].state == PROC_RUNNING) st = "RUNNING";
        if (procs[i].state == PROC_BLOCKED) st = "BLOCKED";
        term_printf("  %-5u %-5u %-17s %-9s %-4u %-7u %-8u %s\n",
            procs[i].pid, procs[i].host_pid, procs[i].name, st,
            procs[i].priority, procs[i].cpu_time,
            procs[i].memory_kb,
            procs[i].activity[0] ? procs[i].activity : "-");
    }
}

process_t *sched_current(void) { return &procs[current_proc]; }
int sched_count(void) { return proc_count; }
