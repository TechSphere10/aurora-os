# AuroraLang: The Unified Intent-Based Language

AuroraLang is a next-generation programming language built specifically for AuroraOS. It abandons legacy paradigms to focus on intent, temporal state, and semantic structure. It is designed to be the only language you need for systems, UI, and data processing.

## Core Philosophy: The 4 Pillars

1.  **Intent Over Implementation**: Code should describe *what* to do, not just *how*. The runtime intelligently handles the *how*.
2.  **Temporal Awareness**: Variables have history. Time is a first-class dimension, eliminating the need for complex logging or undo-redo architectures.
3.  **Semantic Scopes**: Blocks of code imply context (UI batching, Transactions, Security), enforcing system stability at the language level.
4.  **Process Symbiosis**: The language and the OS are one. No "system calls" hidden behind opaque libraries; language constructs map directly to OS resources.

## Language Syntax & Features

### 1. Variables & Strong Typing
AuroraLang uses inference where possible but enforces strong types for safety.

```auroralang
let x = 42                # Integer
const PI = 3.14           # Constant Float
let name = "Aurora"       # String
let active = true         # Boolean

# Type Conversion (Automatic in strings)
print "Value: " + x       # Output: "Value: 42"
```

### 2. Temporal Variables (Time Travel)
One of the most unique features of AuroraLang. Declare a variable as `temporal` to automatically track its state changes.

```auroralang
temporal let sensor = 0
sensor = 10
sensor = 20

# Query past states natively
let past_val = sensor @ -1    # Access previous value (10)
let init_val = sensor @ 0     # Access initial value (0)

# Rollback state
rewind sensor to -1           # Sensor is now 10 again
```

### 3. Semantic Scopes
Scopes define the *behavior* of execution, not just variable visibility.

```auroralang
scope ui_batch {
    window.title = "Loading..."
    progress.value = 50
    # Updates are applied atomically at end of scope to prevent flicker
}

scope atomic {
    # Kernel-level transaction; if any line fails, all changes revert
    file.write("config.sys", data)
    system.registry.update(key, val)
}
```

### 4. Intent-Based Patterns
AuroraLang understands high-level goals.

```auroralang
# Instead of writing a sorting algorithm:
intent "sort users by name ascending"

# Instead of complex file loops:
intent "find all .log files in /var older than 7 days"
```

### 5. Native Concurrency
Parallelism is a keyword, not a library.

```auroralang
parallel {
    task_a()
    task_b()
}

# Wait for specific outcomes
await task_a
```

### 6. Built-in UI Primitives
Frontend code is native to the language, treating UI nodes as data structures.

```auroralang
ui window "My App" {
    layout vertical
    style { background: #333; color: white; }
    
    label "Welcome User"
    
    button "Submit" {
        action: submit_form()
        animate: ripple
    }
}
```

## Why It Is Different

### Problem: Callback Hell & Async Complexity
**Aurora Solution**: Native `parallel` blocks and `temporal` variables remove the need for complex state management logic in async operations.

### Problem: Broken UIs & Flicker
**Aurora Solution**: `scope ui_batch` ensures the OS renderer only draws when the logic is complete.

### Problem: Debugging Mystery Bugs
**Aurora Solution**: With `temporal` types, you can dump the exact history of a variable leading up to a crash without running a debugger.

### Problem: Security
**Aurora Solution**: `scope secure` allows the OS to sandbox specific blocks of code within a single function, restricting file or network access only to those lines.