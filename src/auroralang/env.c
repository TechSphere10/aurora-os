#include "auroralang.h"
#include "string.h"

/* ═══════════════════════════════════════════════════════════════════
   ENVIRONMENT / SCOPE MANAGEMENT
   ═══════════════════════════════════════════════════════════════════ */

void env_push(env_stack_t *env) {
    if (env->top < 15) {
        env->top++;
        env->scopes[env->top].count = 0;
        env->scopes[env->top].parent_idx = env->top - 1;
    }
}

void env_pop(env_stack_t *env) {
    if (env->top >= 0) {
        env->top--;
    }
}

void env_set(env_stack_t *env, const char *name, value_t val, bool is_new) {
    // If not new, try to update existing variable in current or parent scopes
    if (!is_new) {
        if (env_assign(env, name, val)) return;
    }
    // Otherwise (or if not found), define in current scope
    env_scope_t *s = &env->scopes[env->top];
    if (s->count < 64) {
        kstrcpy(s->vars[s->count].name, name);
        s->vars[s->count].val = val;
        s->count++;
    }
}

bool env_get(env_stack_t *env, const char *name, value_t *out) {
    int curr = env->top;
    while (curr >= 0) {
        env_scope_t *s = &env->scopes[curr];
        for (int i = 0; i < s->count; i++) {
            if (kstrcmp(s->vars[i].name, name) == 0) {
                *out = s->vars[i].val;
                return true;
            }
        }
        curr--; // Check parent scope (simple stack approach)
    }
    return false;
}

bool env_assign(env_stack_t *env, const char *name, value_t val) {
    int curr = env->top;
    while (curr >= 0) {
        env_scope_t *s = &env->scopes[curr];
        for (int i = 0; i < s->count; i++) {
            if (kstrcmp(s->vars[i].name, name) == 0) {
                s->vars[i].val = val;
                return true;
            }
        }
        curr--;
    }
    return false;
}