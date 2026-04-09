#include "file.h"
#include "defs.h"
#include "param.h"
#include "proc.h"

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
  struct file ff;
  if(f->ref < 1)
    panic("fileclose");
  if(--f->ref > 0)
    return;
  
  ff = *f;
  f->ref = 0;
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