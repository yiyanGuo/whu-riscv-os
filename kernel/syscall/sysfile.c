#include "defs.h"
#include "file.h"
#include "proc.h"
#include "types.h"

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
  int len, fd;
  uint64 srcva;

  argaddr(1, &srcva);
  argint(2, &len);
  argint(0, &fd);

  return filewrite(fd, srcva, len);
}

uint64 sys_read(void) {
  int len, fd;
  uint64 dstva;

  argint(0, &fd);
  argaddr(1, &dstva);
  argint(2, &len);

  return fileread(fd, dstva, len);
}

uint64
sys_pipe(void)
{
  uint64 fdarray;
  int fd0, fd1;
  int fd[2];
  struct file *rf, *wf;
  struct proc *p;

  p = myproc();
  rf = 0;
  wf = 0;
  fd0 = -1;
  fd1 = -1;

  argaddr(0, &fdarray);
  // 创建pipe
  if(pipealloc(&rf, &wf) < 0)
    goto bad;
  
  // 分配fd
  if((fd0 = fdalloc(rf)) < 0)
    goto bad;
  if((fd1 = fdalloc(wf)) < 0)
    goto bad;

  fd[0] = fd0;
  fd[1] = fd1;
  // 拷贝回用户空间
  if(copyout(p->pagetable, fdarray, (char*)fd, 2*sizeof(int)) < 0)
    goto bad;

  return 0;

bad:
  if(fd0 >= 0)
    p->ofile[fd0] = 0;
  if(fd1 >= 0)
    p->ofile[fd1] = 0;
  if(rf)
    fileclose(rf);
  if(wf)
    fileclose(wf);
  return -1;
}
