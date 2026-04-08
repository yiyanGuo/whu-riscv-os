#include "defs.h"
#include "memlayout.h"
#include "riscv.h"

//创建用户进程页表
pagetable_t
uvmcreate()
{
  pagetable_t pagetable;
  pagetable = (pagetable_t) kalloc();
  if(pagetable == 0)
    return 0;
  memset((char*)pagetable, 0, PGSIZE);
  return pagetable;
}


// copy页表及物理页
int uvmcopy(pagetable_t old, pagetable_t new, uint64 sz) {
  pte_t *pte;
  uint64 pa, i;
  uint flags;
  char *mem;

  for(i = 0; i < sz; i += PGSIZE) {
    if((pte = walk(old, i, 0)) == 0) 
      continue; // 若pte没有分配
    if((*pte & PTE_V) == 0)
      continue; // 物理页没分配
    pa = PTE2PA(*pte);
    flags = PTE_FLAGS(*pte);
    if((mem = kalloc()) == 0) 
      goto err;
    memmove(mem, (char*)pa, PGSIZE);
    if(mappages(new, (uint64)mem, i, PGSIZE, flags)) {
      kfree(mem);
      goto err;
    }
  }
  return 0;

  err:
    uvmunmap(new, 0, i, 1);
    return -1;
}

// 根据虚拟地址释放指定页，将页表项置0
void
uvmunmap(pagetable_t pagetable, uint64 va, uint64 sz, int do_free)
{
  uint64 a, npages;
  pte_t *pte;

  if((va % PGSIZE) != 0)
    panic("uvmunmap: not aligned");
  if((sz % PGSIZE) != 0)
    panic("uvmunmap: size not aligned");

  npages = sz / PGSIZE;
  
  for(a = va; a < va + npages*PGSIZE; a += PGSIZE){
    if((pte = walk(pagetable, a, 0)) == 0) // leaf page table entry allocated?
      continue;   
    if((*pte & PTE_V) == 0)  // has physical page been allocated?
      continue;
    if(do_free){
      uint64 pa = PTE2PA(*pte);
      kfree((void*)pa);
    }
    *pte = 0;
  }
}
