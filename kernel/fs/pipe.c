#include "defs.h"
#include "file.h"

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
