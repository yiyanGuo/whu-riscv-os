/* sysproc.c — 系统调用内核实现（Lab6 任务4）
 *
 * 每个 sys_xxx() 函数是对应系统调用的真正内核实现。
 * 它们不接受参数（参数通过陷阱帧的寄存器传入，用 argint/argaddr 读取），
 * 返回 uint64 类型的结果值。
 */

#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "proc.h"
#include "riscv.h"
#include "types.h"

/* ================================================================
 * sys_getpid — 返回当前进程的 PID
 *
 * 对应的用户接口：int getpid(void)
 *
 * 实现很简单：调用 myproc() 获取当前进程的 PCB，
 * 然后返回它的 pid 字段。
 * ================================================================ */
uint64 sys_getpid(void) {
  /* ================================================================
   * TODO [Lab6-任务4-步骤1]：
   *   调用 myproc() 获取当前进程的 PCB 指针，返回其 pid 字段。
   * ================================================================ */
  return myproc()->pid;
}

/* ================================================================
 * sys_exit (Lab6 扩展)
 *   实现进程退出。简化版：打印退出信息，将进程状态设为 TASK_ZOMBIE，然后切回调度器。
 * ================================================================ */
uint64 sys_exit(void) {
  /* ================================================================
   * TODO [Lab6-任务4-步骤2（可选）]：
   *   实现进程退出。简化版：打印退出信息，将进程状态设为 ZOMBIE，然后切回调度器。
   * ================================================================ */
  struct proc *p;
  p = myproc();
  p->status = TASK_ZOMBIE;
  printf("process %d exit\n", myproc()->pid);
  swtch(&p->context, &mycpu()->context);
  return 0;
}

/* ================================================================
 * sys_write — 向文件描述符写数据
 *
 * 用户接口：int write(int fd, const void *buf, int count)
 * 参数从陷阱帧读取：
 *   fd    = trapframe->a0
 *   buf   = trapframe->a1（用户虚拟地址，不能直接在内核读！）
 *   count = trapframe->a2
 *
 * 简化版：如果 fd==1（标准输出），直接把字符打印到串口。
 * ================================================================ */
uint64 sys_write(void) {
  /* ================================================================
   * TODO [Lab6-任务4-步骤3（进阶）]：
   *   实现简化版 sys_write：
   *   1. int fd = myproc()->trapframe->a0;
   *   2. 获取出参 n（系统调用的第一个参数，可用 argint 拿取），并赋给 p->xstate
   *   3. 打印类似 "Process [pid] exited with code [n]\n"
   *   4. 设置 p->status = TASK_ZOMBIE
   *   5. 调用 swtch 切回调度器：swtch(&p->context, &mycpu()->context);
   * ================================================================ */
  // TODO: 拷贝到内核缓冲区
  struct proc *p;
  int fd;
  int count;
  uint64 va;
  
  p = myproc();
  fd = p->trapframe->a0;
  va = p->trapframe->a1;
  count = p->trapframe->a2;

  for(int i = 0; i < count; i++) {
    uint64 srcva = va + i;
    uint64 pa = walkaddr(p->pagetable, srcva);
    char c;

    if(fd != 1 || pa == 0)
      return -1;

    c = *(char *)(pa + (srcva & (PGSIZE - 1)));
    uart_putc(c);
  }
  return 0;
}

uint64 sys_yield(void) {
  yield();
  return 0;
}

uint64 sys_fork() {
  return kfork();
}

uint64 sys_wait() {
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64 sys_exec() {
  char program_name[MAXNAME];
  argstr(0, program_name, MAXNAME);
  return kexec(program_name);
}
uint64 sys_print0(void) {
  printf("sys_print0 called\n");
  return 0;
}


