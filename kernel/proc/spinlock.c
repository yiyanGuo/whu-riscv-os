#include "defs.h"
#include "riscv.h"
#include "spinlock.h"
#include "proc.h"

void initlock(struct spinlock *lk, char *name) {
  lk->name = name;
  lk->locked = 0;
  lk->cpu = 0;
}

void acquire(struct spinlock *lk) {
  push_off();
  if(holding(lk))
    panic("acquire when holding");
  while(__sync_lock_test_and_set(&lk->locked, 1) != 0);
  // 内存屏障
  __sync_synchronize();

  lk->cpu = mycpu();
}

void release(struct spinlock *lk) {
  if(!holding(lk))
    panic("release when not holding");
  lk->cpu = 0;
  __sync_synchronize();
  __sync_lock_release(&lk->locked);

  pop_off();
}

int holding(struct spinlock *lk) {
  return lk->locked && lk->cpu == mycpu();
}

void push_off() {
  int old = intr_get();
  intr_off();
  if(mycpu()->noff == 0)
    mycpu()->intena = old;
  mycpu()->noff++;
}

void pop_off() {
  struct cpu *c = mycpu();
  if(intr_get())
    panic("pop_off - interruptible");
  if(c->noff < 1)
    panic("pop_off");
  c->noff -= 1;
  if(c->noff == 0 && c->intena)
    intr_on();
}
