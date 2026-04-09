#include "defs.h"
#include "file.h"
#include "types.h"
#include "proc.h"

int pipealloc(struct file **f0, struct file **f1) {
  struct pipe *pi;
  pi = 0;
  *f0 = *f1 = 0;
  // 分配两个文件槽，一个写一个读
  if(((*f0 = filealloc()) == 0) || ((*f1 = filealloc()) == 0)) {
    goto bad;
  }
  
  // 分配缓冲区，pipe本质是一个缓冲区
  if((pi = (struct pipe *)kalloc()) == 0) {
    goto bad;
  }
  pi->nread = 0;
  pi->nwrite = 0;
  pi->readopen = 1;
  pi->writeopen = 1;

  // 初始化f0 f1，让两个文件指向缓冲区
  (*f0)->type = FD_PIPE;
  (*f0)->pipe = pi;
  (*f0)->readable = 1;
  (*f0)->writable = 0;

  (*f1)->type = FD_PIPE;
  (*f1)->pipe = pi;
  (*f1)->readable = 0;
  (*f1)->writable = 1;

  return 0;

  bad:
    if(pi)
      kfree((void *)pi);
    if(*f0)
      fileclose(*f0);
    if(*f1)
      fileclose(*f1);
    return -1;
}

int pipewrite(struct pipe *pi, uint64 src, int n) {
  struct proc *p = myproc();
  uint pipe_size;
  uint used;
  uint free_space;
  uint write_pos;
  uint chunk;
  int written;

  if(n < 0)
    return -1;

  pipe_size = sizeof(pi->data);
  used = pi->nwrite - pi->nread;
  free_space = pipe_size - used;
  if((uint)n > free_space)
    return -1;

  written = 0;
  while(written < n) {
    write_pos = pi->nwrite % pipe_size;
    chunk = pipe_size - write_pos;
    if(chunk > (uint)(n - written))
      chunk = n - written;

    if(copyin(p->pagetable, &pi->data[write_pos], src + written, chunk) < 0)
      return -1;

    pi->nwrite += chunk;
    written += chunk;
  }
  
  return written;
}

int piperead(struct pipe *pi, uint64 dst, int n) {
  struct proc *p = myproc();
  uint pipe_size;
  uint available;
  uint read_pos;
  uint chunk;
  int readn;

  if(n < 0)
    return -1;

  pipe_size = sizeof(pi->data);
  available = pi->nwrite - pi->nread;
  while((uint)n > available && pi->writeopen) {
    available = pi->nwrite - pi->nread;
  }
  if((uint)n > available)
    n = available;

  readn = 0;
  while(readn < n) {
    read_pos = pi->nread % pipe_size;
    chunk = pipe_size - read_pos;
    if(chunk > (uint)(n - readn))
      chunk = n - readn;

    if(copyout(p->pagetable, dst + readn, &pi->data[read_pos], chunk) < 0)
      return -1;

    pi->nread += chunk;
    readn += chunk;
  }    

  return readn;
}
