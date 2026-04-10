#ifndef SPINLOCK_H
#define SPINLOCK_H

#include "types.h"

struct cpu;

struct spinlock {
  uint locked;
  char *name;
  struct cpu *cpu;
};

#endif /* SPINLOCK_H */
