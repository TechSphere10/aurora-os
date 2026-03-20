#include "auroralang.h"
#include "../lib/string.h"

/* ═══════════════════════════════════════════════════════════════════
   ENVIRONMENT / SCOPE MANAGEMENT
   ═══════════════════════════════════════════════════════════════════ */

void env_push(env_t *e) {
    if (e->depth < ENV_MAX_DEPTH - 1) {
        e->depth++;
        e->frames[e->depth].var_count = 0;
    } else {
        term_printf("Runtime Error: Scope stack overflow\n");
    }
}

void env_pop(env_t *e) {
    if (e->depth > 0) {
        e->depth--;
    }
}

/* Set a variable in the current scope (declaration) */
bool env_set(env_t *e, const char *name, value_t val, bool is_const) {
    env_frame_t *frame = &e->frames[e->depth];

    /* Check for redefinition in current scope */
    for (int i = 0; i < frame->var_count; i++) {
        if (kstrcmp(frame->vars[i].name, name) == 0) {
            if (frame->vars[i].is_const) {
                term_printf("Runtime Error: Cannot redeclare constant '%s'\n", name);
                return false;
            }
            // Update existing
            frame->vars[i].val = val;
            frame->vars[i].is_const = is_const;
            return true;
        }
    }

    if (frame->var_count < ENV_MAX_VARS) {
        env_var_t *var = &frame->vars[frame->var_count++];
        kstrncpy(var->name, name, 63);
        var->val = val;
        var->is_const = is_const;
        return true;
    } else {
        term_printf("Runtime Error: Variable limit reached in scope\n");
        return false;
    }
}

/* Get a variable from the nearest scope */
bool env_get(env_t *e, const char *name, value_t *out) {
    for (int d = e->depth; d >= 0; d--) {
        env_frame_t *frame = &e->frames[d];
        for (int i = 0; i < frame->var_count; i++) {
            if (kstrcmp(frame->vars[i].name, name) == 0) {
                *out = frame->vars[i].val;
                return true;
            }
        }
    }
    return false;
}

/* Assign to an existing variable in the nearest scope */
bool env_assign(env_t *e, const char *name, value_t val) {
    for (int d = e->depth; d >= 0; d--) {
        env_frame_t *frame = &e->frames[d];
        for (int i = 0; i < frame->var_count; i++) {
            if (kstrcmp(frame->vars[i].name, name) == 0) {
                if (frame->vars[i].is_const) {
                    term_printf("Runtime Error: Assignment to constant '%s'\n", name);
                    return false;
                }
                frame->vars[i].val = val;
                return true;
            }
        }
    }
    return false;
}