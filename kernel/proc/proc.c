/* proc.c — 进程管理（Lab5 任务1、3、4）
 *
 * 本文件实现了进程生命周期管理的核心逻辑：
 *   - procinit()   : 初始化进程表
 *   - allocproc()  : 为新进程分配 PCB
 *   - scheduler()  : 调度器主循环（无限轮询、找到就绪进程就运行）
 *   - yield()      : 当前进程主动放弃 CPU（配合时钟中断使用）
 */

#include "proc.h"
#include "defs.h"
#include "memlayout.h"
#include "param.h"
#include "riscv.h"
#include "types.h"

extern char trampoline[];
extern void forkret();
extern pagetable_t kernel_pagetable;
/* 全局进程表和 CPU 描述符（在 proc.h 中 extern 声明）*/
struct proc proc[NPROC];
struct cpu cpus[NCPU];

/* 进程 ID 计数器（每次 allocpid 返回后递增）*/
static int nextpid = 1;

//第一个用户进程
struct proc *initproc;

/* ================================================================
 * mycpu — 获取当前 CPU 核心的 cpu 结构指针
 *
 * 实现方式：读取 tp 寄存器（在 start.c 中被设置为 hartid）
 * ================================================================ */
struct cpu *mycpu(void) {
  int hartid = r_tp();
  return &cpus[hartid];
}

/* ================================================================
 * myproc — 获取当前 CPU 上正在运行的进程的 PCB 指针
 * ================================================================ */
struct proc *myproc(void) { return mycpu()->proc; }

/* ================================================================
 * allocpid — 分配一个唯一的进程 ID
 * ================================================================ */
int allocpid(void) { return nextpid++; }

/* ================================================================
 * procinit — 初始化进程表（内核启动时调用一次）
 *
 * 任务：将进程表中所有条目的状态初始化为 TASK_FREE。
 * ================================================================ */
void procinit(void) {
  /* ================================================================
   * TODO [Lab5-任务1-步骤1]：
   *   遍历 proc[] 数组，将每个进程的 status 置为 TASK_FREE。
   * ================================================================ */
  struct proc *p;
  for(p = proc; p < &proc[NPROC]; p++) {
    p->status = TASK_FREE;
    p->kstack = KSTACK((int)(p - proc));
  }
}

/* ================================================================
 * allocproc — 在进程表中找一个空槽并初始化
 *
 * 返回：指向已初始化的 PCB 的指针；若进程表满，返回 0。
 *
 * 初始化内容：
 *   - 分配 pid
 *   - 将状态从 TASK_FREE 改为 TASK_ALLOCATED
 *   - 分配 trapframe 页（用于保存用户寄存器）
 *   - 初始化内核 context（ra 设为某个"进程首次被调度时跳入的地址"）
 * ================================================================ */
struct proc *allocproc(void) {
  struct proc *p;

  /* 在进程表中寻找一个 TASK_FREE 的槽位 */
  for (p = proc; p < &proc[NPROC]; p++) {
    if (p->status == TASK_FREE)
      goto found;
  }
  return 0; /* 进程表已满 */

found:
  /* ================================================================
   * TODO [Lab5-任务1-步骤2]：
   *   完成进程初始化：
   *   1. 分配 pid：调用 allocpid()
   *   2. 分配 trapframe 页：调用 kalloc()；若失败则将状态恢复为 TASK_FREE 并返回0
   *   3. 将进程状态设为 TASK_ALLOCATED
   * ================================================================ */
  p->pid = allocpid();
  // trapframe
  if((p->trapframe = (struct trapframe *)kalloc()) == 0) {
    p->status = TASK_FREE;
    return 0;
  }
  // page table
  pagetable_t pagetable;
  pagetable = uvmcreate();
  if(pagetable == 0) return 0;
  if(mappages(pagetable, (uint64)trampoline, TRAMPOLINE, PGSIZE, PTE_R | PTE_X) < 0) {
    return 0;
  }
  if(mappages(pagetable, (uint64)p->trapframe, TRAPFRAME, PGSIZE, PTE_R | PTE_W) < 0) {
    return 0;
  }
  p->pagetable = pagetable;
  
  // set context
  memset((char*)&p->context, 0, sizeof(p->context));
  p->context.ra = (uint64)forkret;
  p->context.sp = p->kstack + PGSIZE;

  // kstack
  uint64 phy_kstack = (uint64)kalloc();
  if(mappages(kernel_pagetable, (uint64)phy_kstack, p->kstack, PGSIZE, PTE_R | PTE_W) < 0) {
    return 0;
  }

  p->status = TASK_ALLOCATED;
  return p;
}

/* ================================================================
 * scheduler — 调度器主循环（永不返回！）
 *
 * 这是操作系统的"上帝"：它在所有进程之间无限轮转，
 * 当看到一个 TASK_READY 的进程时，就把 CPU 交给它。
 *
 * 流程：
 *   for 每次循环:
 *     1. 打开全局中断（防止系统无法接收时钟信号而死锁）
 *     2. 遍历进程表，找到 TASK_READY 的进程
 *     3. 将该进程标记为 TASK_RUNNING
 *     4. 调用 swtch，从调度器上下文切换到进程的内核上下文
 *     5. 当进程放弃 CPU（yield/sleep/exit）后，swtch 返回到这里
 *     6. 清除 mycpu()->proc，继续找下一个
 * ================================================================ */
void scheduler(void) {
  struct proc *p;
  struct cpu *c = mycpu();

  c->proc = 0;

  for (;;) {
    /* 必须打开中断！否则时钟信号无法到达，调度无法触发 */
    intr_on();

    for (p = proc; p < &proc[NPROC]; p++) {
      /* ================================================================
       * TODO [Lab5-任务3]：
       *   完成调度器核心逻辑：
       *   1. 检查 p->status == TASK_READY
       *   2. 将状态改为 TASK_RUNNING
       *   3. 将 c->proc 设为 p
       *   4. 调用 swtch 切换到 p 的上下文：swtch(&c->context, &p->context)
       *   5. swtch 返回后（进程放弃了CPU），清零 c->proc
       * ================================================================ */
      if(p->status != TASK_READY) continue;
      p->status = TASK_RUNNING;
      c->proc = p;
      swtch(&c->context, &p->context);
      c->proc = 0;      
    }
  }
}

/* ================================================================
 * yield — 当前进程主动放弃 CPU（由时钟中断处理函数调用）
 *
 * 过程：将自己的状态从 TASK_RUNNING 改回 TASK_READY，然后切回调度器。
 * ================================================================ */
void yield(void) {
  struct proc *p = myproc();
  struct cpu *c = mycpu();
  /* ================================================================
   * TODO [Lab5-任务4]：
   *   1. 将进程状态改为 TASK_READY
   *   2. 调用 swtch 切回调度器上下文：swtch(&p->context, &mycpu()->context)
   *
   *   思考：为什么是 "进程 → 调度器" 而不是 "进程A → 进程B" 直接切换？
   * ================================================================ */
  p->status = TASK_READY;
  swtch(&p->context, &c->context);
}

// A fork child's very first scheduling by scheduler()
// will swtch to forkret.
void
forkret(void)
{
  usertrapret();
  while(1);
}

extern pagetable_t kernel_pagetable;
extern char trampoline[];

static uint8 initcode[] = {
    0x73, 0x00, 0x00, 0x00,   // ecall
    0x73, 0x00, 0x00, 0x00,   // ecall
    0x6f, 0x00, 0x00, 0x00,   // j .
};

int userinit(){
  char *mem;
  pte_t *pte;

  if((initproc = allocproc()) == 0) return -1;
  


  // 测试用户态代码执行
  if((mem = (char*)kalloc()) == 0) return -1;
  memset(mem, 0, PGSIZE);
  if(mappages(initproc->pagetable, (uint64)mem, 0, PGSIZE, PTE_R | PTE_U | PTE_X) < 0 ){
    return -1;
  }
  
  //加载代码
  memmove(mem, (char*)initcode, sizeof(initcode));

  initproc->trapframe->epc = 0;
  initproc->trapframe->sp = PGSIZE;
  initproc->sz = PGSIZE;

  initproc->status = TASK_READY;
  return 0;
}