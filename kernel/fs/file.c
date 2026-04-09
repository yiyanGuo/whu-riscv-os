#include "file.h"
#include "defs.h"
#include "param.h"
#include "proc.h"
#include "types.h"

struct {
    struct file files[NFILE];
} ftable;

struct file* filealloc(void) {
  for(int i = 0; i < NFILE; i++) {
    if(ftable.files[i].ref == 0) {
      ftable.files[i].ref = 1;
      return &ftable.files[i];
    }
  }
  return 0;
}

struct file* filedup(struct file *f) {
  if(f->ref < 1)
    panic("filedup");
  f->ref++;
  return f;
}

void fileclose(struct file *f) {
//   struct file ff;
//   if(f->ref < 1)
//     panic("fileclose");
//   if(--f->ref > 0)
//     return;
  
//   ff = *f;
//   f->ref = 0;
}

int fdalloc(struct file *f) {
  struct proc *p = myproc();
  for(int fd = 0; fd < NOFILE; fd++) {
    if(p->ofile[fd] == 0){
      p->ofile[fd] = f;
        return fd;
      }
  }
  return -1;
}

int filewrite(int fd, uint64 src, int len) {
  struct proc *p = myproc();
  struct file *f;
  int ret = 0;
  char buffer[1024]; // tmp

  // 根据fd找 file
  if(fd < 0 || fd >= NOFILE || (f = p->ofile[fd]) == 0)
    return -1;
  
  // 权限检查
  if(f->writable == 0)
    return -1;
  
  // 根据file类型分发
  if(f->type == FD_PIPE){
    ret = pipewrite(f->pipe, src, len);
  } else if(fd == 1) {
    copyin(p->pagetable, buffer, src, len);
    buffer[len] = '\0';
    printf(buffer);
  }

  return ret;
}

int fileread(int fd, uint64 dst, int len) {
  struct proc *p = myproc();
  struct file *f;
  int ret = 0;

  // 根据fd找file
  if(fd < 0 || fd >= NOFILE || (f = p->ofile[fd]) == 0)
    return -1;
  // 根据file类型分发
  if(f->type == FD_PIPE){
    ret = piperead(f->pipe, dst, len);
  } else {

  }

  return ret;
}