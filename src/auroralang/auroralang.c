#include "auroralang.h"
#include "string.h"
#include "../kernel/kernel.h"

/* ═══════════════════════════════════════════════════════════════════
   AURORALANG  –  AST-Walking Interpreter
   ═══════════════════════════════════════════════════════════════════ */

/* ── Interpreter Init ────────────────────────────────────────────── */
void interp_init(interp_t *it, ast_t *ast) {
    kmemset(it, 0, sizeof(interp_t));
    it->ast = ast;
    env_push(&it->env); // Push global scope
}

/* ── Helper: Value to String ─────────────────────────────────────── */
static void val_to_str(value_t *v, char *buf, int size) {
    if (v->type == VAL_INT) kitoa(v->ival, buf, 10);
    else if (v->type == VAL_FLOAT) {
        int i = (int)v->fval;
        int f = (int)((v->fval - (double)i) * 1000.0);
        if (f < 0) f = -f;
        char ib[32], fb[32]; kitoa(i, ib, 10); kitoa(f, fb, 10);
        ksnprintf(buf, size, "%s.%s", ib, fb);
    }
    else if (v->type == VAL_STRING) kstrncpy(buf, v->sval, size - 1);
    else if (v->type == VAL_BOOL) kstrcpy(buf, v->bval ? "true" : "false");
    else kstrcpy(buf, "null");
}

value_t interp_exec(interp_t *it, int node_idx);

/* ── Expression Evaluation ───────────────────────────────────────── */
static value_t eval_expr(interp_t *it, int node_idx) {
    ast_node_t *node = &it->ast->nodes[node_idx];
    value_t result = { .type = VAL_NULL };

    switch (node->type) {
        case NODE_INT_LIT:
            result.type = VAL_INT;
            result.ival = node->ival;
            break;
        case NODE_STR_LIT:
            result.type = VAL_STRING;
            kstrcpy(result.sval, node->sval);
            break;
        case NODE_FLOAT_LIT:
            result.type = VAL_FLOAT;
            result.fval = node->fval;
            break;
        case NODE_BOOL_LIT:
            result.type = VAL_BOOL;
            result.bval = node->ival ? true : false;
            break;
        case NODE_NULL_LIT:
            result.type = VAL_NULL;
            break;
            
        case NODE_IDENT:
            if (!env_get(&it->env, node->sval, &result)) {
                result.type = VAL_ERROR;
                ksnprintf(result.sval, sizeof(result.sval), "Undefined variable '%s'", node->sval);
            }
            break;
        case NODE_BINOP: {
            value_t left = eval_expr(it, node->children[0]);
            value_t right = eval_expr(it, node->children[1]);
            
            if (left.type == VAL_ERROR) return left;
            if (right.type == VAL_ERROR) return right;

            // Operator: + (Addition and String Concatenation)
            if (kstrcmp(node->sval, "+") == 0) {
                // If either operand is a string, perform concatenation
                if (left.type == VAL_STRING || right.type == VAL_STRING) {
                    result.type = VAL_STRING;
                    char lbuf[128], rbuf[128];
                    val_to_str(&left, lbuf, sizeof(lbuf));
                    val_to_str(&right, rbuf, sizeof(rbuf));

                    // Concatenate safely
                    kstrncpy(result.sval, lbuf, 128);
                    if (kstrlen(result.sval) + kstrlen(rbuf) < 255) kstrcat(result.sval, rbuf);
                } 
                // Numeric Addition
                else if (left.type == VAL_INT && right.type == VAL_INT) {
                    result.type = VAL_INT;
                    result.ival = left.ival + right.ival;
                }
                else {
                    result.type = VAL_ERROR;
                    kstrcpy(result.sval, "Invalid types for + operator");
                }
            }
            // Operator: - (Subtraction)
            else if (kstrcmp(node->sval, "-") == 0) {
                if (left.type == VAL_INT && right.type == VAL_INT) {
                    result.type = VAL_INT;
                    result.ival = left.ival - right.ival;
                }
            }
            // Operator: * (Multiplication)
            else if (kstrcmp(node->sval, "*") == 0) {
                if (left.type == VAL_INT && right.type == VAL_INT) {
                    result.type = VAL_INT;
                    result.ival = left.ival * right.ival;
                }
            }
            // Operator: / (Division)
            else if (kstrcmp(node->sval, "/") == 0) {
                if (left.type == VAL_INT && right.type == VAL_INT) {
                     if (right.ival == 0) {
                        result.type = VAL_ERROR;
                        kstrcpy(result.sval, "Division by zero");
                     } else {
                        result.type = VAL_INT;
                        result.ival = left.ival / right.ival;
                     }
                }
            }
            // Comparisons
            else if (kstrcmp(node->sval, "==") == 0) {
                result.type = VAL_BOOL;
                if (left.type == VAL_INT && right.type == VAL_INT) result.bval = (left.ival == right.ival);
                else if (left.type == VAL_STRING && right.type == VAL_STRING) result.bval = (kstrcmp(left.sval, right.sval) == 0);
                else result.bval = false;
            }
            else if (kstrcmp(node->sval, "!=") == 0) {
                result.type = VAL_BOOL;
                if (left.type == VAL_INT && right.type == VAL_INT) result.bval = (left.ival != right.ival);
                else result.bval = true;
            }
            else if (kstrcmp(node->sval, "<") == 0) {
                result.type = VAL_BOOL;
                if (left.type == VAL_INT && right.type == VAL_INT) result.bval = (left.ival < right.ival);
                else result.bval = false;
            }
            else if (kstrcmp(node->sval, ">") == 0) {
                result.type = VAL_BOOL;
                if (left.type == VAL_INT && right.type == VAL_INT) result.bval = (left.ival > right.ival);
                else result.bval = false;
            }
            
            // Logical
            else if (kstrcmp(node->sval, "&&") == 0) {
                result.type = VAL_BOOL;
                bool l = (left.type == VAL_BOOL) ? left.bval : (left.type == VAL_INT ? left.ival : false);
                bool r = (right.type == VAL_BOOL) ? right.bval : (right.type == VAL_INT ? right.ival : false);
                result.bval = l && r;
            }
            else if (kstrcmp(node->sval, "||") == 0) {
                result.type = VAL_BOOL;
                bool l = (left.type == VAL_BOOL) ? left.bval : (left.type == VAL_INT ? left.ival : false);
                bool r = (right.type == VAL_BOOL) ? right.bval : (right.type == VAL_INT ? right.ival : false);
                result.bval = l || r;
            }
            break;
        }
        
        case NODE_UNOP: {
             value_t op = eval_expr(it, node->children[0]);
             if (kstrcmp(node->sval, "!") == 0) {
                 result.type = VAL_BOOL;
                 if (op.type == VAL_BOOL) result.bval = !op.bval;
                 else if (op.type == VAL_INT) result.bval = !op.ival;
                 else result.bval = false;
             } else if (kstrcmp(node->sval, "-") == 0) {
                 if (op.type == VAL_INT) { result.type = VAL_INT; result.ival = -op.ival; }
                 else if (op.type == VAL_FLOAT) { result.type = VAL_FLOAT; result.fval = -op.fval; }
             }
             break;
        }
        
        case NODE_CAST: {
             value_t val = eval_expr(it, node->children[0]);
             if (kstrcmp(node->sval, "str") == 0) {
                 result.type = VAL_STRING;
                 val_to_str(&val, result.sval, sizeof(result.sval));
             }
             // Add int(), float() here
             break;
        }
        
        case NODE_CALL: {
             /* Find function definition */
             int func_idx = -1;
             for (int i = 0; i < it->func_count; i++) {
                 if (kstrcmp(it->funcs[i].name, node->sval) == 0) {
                     func_idx = i;
                     break;
                 }
             }
             if (func_idx == -1) {
                 result.type = VAL_ERROR;
                 ksnprintf(result.sval, 64, "Unknown function '%s'", node->sval);
                 break;
             }
             
             /* Evaluate args */
             value_t args[8];
             int arg_count = 0;
             for (int i = 0; i < node->child_count && i < 8; i++) {
                 args[i] = eval_expr(it, node->children[i]);
                 arg_count++;
             }
             
             /* Call setup */
             env_push(&it->env);
             func_def_t *fd = &it->funcs[func_idx];
             for (int i = 0; i < fd->param_count && i < arg_count; i++) {
                 ast_node_t *pnode = &it->ast->nodes[fd->params[i]];
                 env_set(&it->env, pnode->sval, args[i], false);
             }
             
             /* Execute */
             interp_exec(it, fd->body);
             
             /* Teardown */
             env_pop(&it->env);
             if (it->returning) {
                 result = it->return_val;
                 it->returning = false;
             }
             break;
        }

        default:
            result.type = VAL_ERROR;
            kstrcpy(result.sval, "Unsupported expression");
            break;
    }
    return result;
}

/* ── Statement Execution ─────────────────────────────────────────── */
value_t interp_exec(interp_t *it, int node_idx) {
    ast_node_t *node = &it->ast->nodes[node_idx];
    value_t result = { .type = VAL_NULL };

    switch (node->type) {
        case NODE_PROGRAM:
        case NODE_BLOCK:
            for (int i = 0; i < node->child_count; i++) {
                result = interp_exec(it, node->children[i]);
                if (result.type == VAL_ERROR) {
                    term_printf("Runtime Error: %s\n", result.sval);
                    return result;
                }
                if (it->returning || it->breaking || it->continuing) break;
            }
            break;

        case NODE_LET: {
            value_t val = eval_expr(it, node->children[0]);
            if (val.type == VAL_ERROR) return val;
            env_set(&it->env, node->sval, val, false);
            break;
        }

        case NODE_ASSIGN: {
            value_t val = eval_expr(it, node->children[0]);
            if (val.type == VAL_ERROR) return val;
            if (!env_assign(&it->env, node->sval, val)) {
                result.type = VAL_ERROR;
                ksnprintf(result.sval, 64, "Cannot assign to '%s'", node->sval);
            }
            break;
        }

        case NODE_FUNCTION: {
            if (it->func_count < MAX_FUNCS) {
                func_def_t *fd = &it->funcs[it->func_count++];
                kstrcpy(fd->name, node->sval);
                fd->body = node->children[node->child_count - 1]; // Last child is body
                fd->param_count = node->child_count - 1;
                for(int i=0; i<fd->param_count; i++) {
                    fd->params[i] = node->children[i];
                }
                fd->used = true;
            }
            break;
        }

        case NODE_IF: {
            value_t cond = eval_expr(it, node->children[0]);
            bool is_true = (cond.type == VAL_BOOL && cond.bval) || (cond.type == VAL_INT && cond.ival != 0);
            
            if (is_true) {
                interp_exec(it, node->children[1]); // Then block
            } else if (node->child_count > 2) {
                interp_exec(it, node->children[2]); // Else block
            }
            break;
        }

        case NODE_WHILE: {
            while (true) {
                value_t cond = eval_expr(it, node->children[0]);
                if (!((cond.type == VAL_BOOL && cond.bval) || (cond.type == VAL_INT && cond.ival != 0)))
                    break;
                interp_exec(it, node->children[1]); // Body
                if (it->returning) break;
                if (it->breaking) { it->breaking = false; break; }
                if (it->continuing) { it->continuing = false; continue; }
            }
            break;
        }
        
        case NODE_RETURN: {
            if (node->child_count > 0) {
                it->return_val = eval_expr(it, node->children[0]);
            } else {
                it->return_val.type = VAL_NULL;
            }
            it->returning = true;
            break;
        }

        case NODE_PRINT: {
            value_t val = eval_expr(it, node->children[0]);
            if (val.type == VAL_ERROR) return val;
            char buf[256];
            val_to_str(&val, buf, sizeof(buf));
            term_printf("%s\n", buf);
            break;
        }

        // TODO: Add other statement types
        default:
            // It might be an expression statement
            return eval_expr(it, node_idx);
    }
    return result;
}

/* ── Public API ────────────────────────────────────────────────────── */
void aurora_runtime_init(void) {
    // Initialize any global state for the runtime
}

int aurora_run_string(const char *code) {
    lexer_t l;
    parser_t p;
    ast_t a;
    interp_t i;

    // 1. Lex
    lexer_init(&l, code);
    lexer_tokenize(&l);

    // 2. Parse
    p.lex = &l;
    p.ast = &a;
    p.had_error = false;
    a.count = 0;
    int root = parse_program(&p);
    if (p.had_error) {
        term_printf("Parse Error: %s\n", p.error);
        return -1;
    }

    // 3. Interpret
    interp_init(&i, &a);
    interp_exec(&i, root);

    return 0;
}