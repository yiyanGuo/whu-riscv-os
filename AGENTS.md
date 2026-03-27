# AGENTS.md — WHU RISC-V OS Lab Skeleton

## Overview

This is a bare-metal RISC-V operating system kernel for educational purposes (8 progressive labs).
The kernel runs on QEMU's virt platform, loaded at physical address `0x80000000`.

## Build Commands

```bash
# Build the kernel
make

# Build and run in QEMU (nographic mode)
# Exit QEMU: Ctrl+A, then X
make run

# Build and start QEMU waiting for GDB connection (port 1234)
make debug

# Clean build artifacts
make clean

# Debug with GDB
gdb-multiarch kernel.elf
(gdb) target remote :1234
(gdb) break _entry
(gdb) continue
```

There is **no test framework** — verification is done by running `make run` and checking console output.

## Code Style Guidelines

### Header Files
- Use header guards: `#ifndef XXX_H`, `#define XXX_H`, `#endif /* XXX_H */`
- Group includes: `"types.h"` (base types), `"defs.h"` (function declarations), then others
- Struct forward declarations in defs.h before use
- Order: `types.h` → `defs.h` → module-specific headers

### Types (defined in `kernel/include/types.h`)
```c
typedef unsigned int   uint;
typedef unsigned short ushort;
typedef unsigned char  uchar;
typedef unsigned char  uint8;
typedef unsigned short uint16;
typedef unsigned int   uint32;
typedef unsigned long  uint64;
typedef uint64 pde_t;
typedef uint64 pte_t;
typedef uint64 *pagetable_t;
```

### Naming Conventions
- Functions: `lowercase_with_underscores` (e.g., `uart_putc`, `kalloc`)
- Types/Structs: `lowercase_with_underscores` (e.g., `struct proc`, `struct context`)
- Enums: `UPPERCASE_WITH_UNDERSCORES` (e.g., `TASK_FREE`, `TASK_RUNNING`)
- Constants/Macros: `UPPERCASE_WITH_UNDERSCORES` (e.g., `PGSIZE`, `PHYSTOP`)
- Global variables: `lowercase_with_underscores` (e.g., `free_mem_list`)
- Static variables: `lowercase_with_underscores` (e.g., `nextpid`)

### TODO Format
All TODO placeholders follow this exact format:
```c
/* TODO [LabX-任务X-步骤X]：
 *   Description of what to implement
 * ================================================================ */
```

### MMIO Register Access
Always use `volatile` for memory-mapped I/O registers:
```c
#define UART0_BASE 0x10000000L
#define Reg(offset) ((volatile unsigned char *)(UART0_BASE + (offset)))
```

### Inline Assembly (riscv.h)
Use GCC inline assembly syntax with `asm volatile`:
```c
static inline uint64 r_mstatus() {
  uint64 x;
  asm volatile("csrr %0, mstatus" : "=r"(x));
  return x;
}
```

### Control Flow
- Use `if (condition) goto label;` pattern for error handling exit points
- Always end non-returning functions with infinite loop: `while (1) ;`
- Mark noreturn functions: `__attribute__((noreturn))`

### Comments
- Chinese comments throughout (educational context)
- File header comment explaining purpose
- Section separators: `/* ================================================================ */`
- NO comments in implementation code unless pedagogical

### Error Handling
- Use `panic(char *msg)` for unrecoverable kernel errors
- Return 0/`NULL` for allocation failures
- Return -1 for error conditions where appropriate

### Struct Definitions Order (proc.h, defs.h)
1. Forward struct declarations (if needed)
2. Enums
3. Struct definitions
4. Global extern declarations
5. Function declarations grouped by module

### Inline Functions in Headers
Static inline functions for CSR access are defined in `riscv.h`:
```c
static inline uint64 r_satp(void) { ... }
static inline void w_satp(uint64 x) { ... }
```

## Project Structure

```
kernel/
├── include/
│   ├── types.h       # Base type definitions
│   ├── param.h       # System parameters (NPROC, NCPU, etc.)
│   ├── memlayout.h   # Memory/device address constants
│   ├── riscv.h       # CSR access inline functions
│   ├── defs.h        # All kernel function declarations
│   └── proc.h        # Process-related structs
├── boot/
│   ├── entry.S       # Kernel entry point (_entry)
│   ├── start.c       # M-Mode initialization
│   └── main.c        # Kernel main (start_main)
├── driver/
│   ├── uart.c        # UART MMIO driver
│   └── console.c     # printf implementation
├── mm/
│   ├── kalloc.c      # Physical page allocator
│   └── vm.c          # Virtual memory/page tables
├── trap/
│   ├── kernelvec.S   # Kernel trap vector
│   ├── trap.c        # Trap handling dispatcher
│   └── timervec.S    # Timer interrupt vector
├── proc/
│   ├── proc.c        # Process management
│   └── swtch.S       # Context switching
├── syscall/
│   ├── syscall.c     # System call dispatcher
│   └── sysproc.c     # Process-related syscalls
└── fs/
    ├── bio.c         # Block buffer layer
    └── fs.c          # Filesystem core
user/
└── usys.S            # User syscall stubs
kernel.ld              # Linker script
Makefile               # Build system
```

## Compiler Flags

```
-nostdlib -fno-builtin -mcmodel=medany
-march=rv64gc -mabi=lp64d -g -Wall -ffreestanding
```

## Clangd Configuration

The `.clangd` file configures IDE support with:
- Target: `riscv64-unknown-elf`
-march=rv64gc -mabi=lp64d -ffreestanding
- Include path: `kernel/include`

## Lab Completion Pattern

Each lab adds new files without modifying previous labs' completed code:
1. Add new `.c`/`.S` files to `SRCS` in Makefile
2. Add function declarations to `defs.h`
3. Call new initialization functions from `start_main()`

## Important Addresses

- Kernel load address: `0x80000000`
- UART0 (QEMU virt): `0x10000000`
- Physical memory limit (PHYSTOP): `0x88000000` (128MB)
- Page size: `4096` bytes
