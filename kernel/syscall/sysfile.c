#include "defs.h"
#include "file.h"
#include "proc.h"
#include "types.h"

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
  if(pipealloc(&rf, &wf) < 0)
    goto bad;

  if((fd0 = fdalloc(rf)) < 0)
    goto bad;
  if((fd1 = fdalloc(wf)) < 0)
    goto bad;

  fd[0] = fd0;
  fd[1] = fd1;
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
