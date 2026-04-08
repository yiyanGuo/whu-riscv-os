/* syscall.c — 系统调用分发（Lab6 任务3）
 *
 * 当用户程序执行 ecall 并由 usertrap() 捕获后，
 * 调用本文件的 syscall() 函数进行分发：
 *   1. 从陷阱帧读取系统调用号（a7 寄存器的值）
 *   2. 在函数指针表中查找对应的内核实现函数
 *   3. 调用该函数，将返回值写回陷阱帧的 a0 寄存器
 */

#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "proc.h"
#include "riscv.h"
#include "types.h"

/* 系统调用号常量定义 */
#define SYS_print0 0
#define SYS_fork 1
#define SYS_exit 2
#define SYS_wait 3
#define SYS_getpid 11
#define SYS_sbrk 12
#define SYS_yield 13
#define SYS_exec 14
#define SYS_write 16

/* 获取定义长度的宏 */
#define NELEM(x) (sizeof(x) / sizeof((x)[0]))

static uint64
argraw(int n)
{
  struct proc *p = myproc();
  switch (n) {
  case 0:
    return p->trapframe->a0;
  case 1:
    return p->trapframe->a1;
  case 2:
    return p->trapframe->a2;
  case 3:
    return p->trapframe->a3;
  case 4:
    return p->trapframe->a4;
  case 5:
    return p->trapframe->a5;
  }
  panic("argraw");
  return -1;
}

// Fetch the nth 32-bit system call argument.
void
argint(int n, int *ip)
{
  *ip = argraw(n);
}

// Retrieve an argument as a pointer.
// Doesn't check for legality, since
// copyin/copyout will do that.
void
argaddr(int n, uint64 *ip)
{
  *ip = argraw(n);
}

void
argstr(int n, char *buf, int max){
  uint64 va0, pa0, srcva, slice_size;
  int got_null = 0;
  struct proc *p = myproc();
  argaddr(n, &srcva);

  if(max <= 0)
    return;

  while(max > 0 && !got_null) {
    va0 = PGROUNDDOWN(srcva);
    pa0 = walkaddr(p->pagetable, va0);
    if(pa0 == 0)
      break;
    slice_size = PGSIZE - (srcva - va0);
    if(slice_size > max)
      slice_size = max;
    
    char *p = (char*)(pa0 + (srcva - va0));
    while(slice_size--) {
      *buf = *p;
      if(*p == '\0') {
        got_null = 1;
        break;
      }
      buf++;
      p++;
      max--;
    }
    srcva = va0 + PGSIZE;
  }

  if(!got_null)
    *buf = '\0';
}

/* ================================================================
 * TODO [Lab6-任务3-步骤1]：
 *   完善系统调用函数指针表 syscalls[]。
 *
 *   工作原理：
 *     syscalls[11] = sys_getpid
 *     当用户程序将 a7=11 并执行 ecall 时，
 *     syscall() 会调用 syscalls[11]()，即 sys_getpid()。
 *
 *   目前只实现 sys_getpid，其余留空（NULL）。
 *   后续可按需添加更多系统调用。
 * ================================================================ */
static uint64 (*syscalls[20])(void) = {
    [SYS_getpid] = sys_getpid,
    [SYS_exit]   = sys_exit,
    [SYS_print0] = sys_print0,
    [SYS_write]  = sys_write,
    [SYS_yield]  = sys_yield,
    [SYS_fork]   = sys_fork,
    [SYS_wait]   = sys_wait,
    [SYS_exec]   = sys_exec
};

/* ================================================================
 * syscall — 系统调用分发主函数（由 usertrap 调用）
 * ================================================================ */
void syscall(void) {
  struct proc *p = myproc();

  /* 从陷阱帧读取系统调用号（用户在 a7 寄存器中填入的值）*/
  int num = p->trapframe->a7;
  /* ================================================================
   * TODO [Lab6-任务3-步骤2]：
   *   1. 检查 num 是否在合法范围内（1 <= num < NELEM(syscalls)），
   *      且 syscalls[num] 不为 NULL。
   *   2. 若合法，调用 syscalls[num]()，
   *      将返回值存入 p->trapframe->a0（用户程序会从 a0 读取返回值）。
   *   3. 若非法，打印错误并将 p->trapframe->a0 = -1（返回错误码）。
   * ================================================================ */
  if (num >= 0 && num < NELEM(syscalls) && syscalls[num] != 0) {
    p->trapframe->a0 = syscalls[num]();
  } else {
    printf("syscall: unknown num %d\n", num);
    p->trapframe->a0 = (uint64)-1;
  }
}
