/* defs.h — 内核函数声明汇总
 *
 * 每完成一个实验，将新实现的函数签名添加到对应区域。
 * 这是整个内核各模块之间"相互认识"的汇总名册。
 */
#ifndef DEFS_H
#define DEFS_H

#include "types.h"

struct proc;
struct context;
struct trapframe;
struct buf;
struct inode;
struct dirent;
struct file;
struct pipe;


int strlen(const char *str);
int streq(const char *str1, const char *str2);

/* ======================================================
 * Lab1 新增：uart 串口驱动
 * 文件：kernel/driver/uart.c
 * ====================================================== */
void uart_putc(char c);
void uart_puts(char *s);

/* ======================================================
 * Lab2 新增：内核 printf
 * 文件：kernel/driver/console.c
 * ====================================================== */
void printf(char *fmt, ...);
void clear_screen(void);
void panic(char *msg) __attribute__((noreturn));

/* ======================================================
 * Lab3 新增：物理内存分配器
 * 文件：kernel/mm/kalloc.c
 * ====================================================== */
void kinit(void);
void *kalloc(void);
void kfree(void *pa);
int memset(char *sa, char val, uint64 size);
int memmove(char *dest, char *src, uint64 size);
/* ======================================================
 * Lab3 新增：虚拟内存 / 页表
 * 文件：kernel/mm/vm.c
 * ====================================================== */
void kvmininit(void);
void kvminithart(void);
pte_t *walk(pagetable_t pagetable, uint64 va, int alloc);
int mappages(pagetable_t pagetable, uint64 pa, uint64 va, uint64 size,
             int perm);
uint64 walkaddr(pagetable_t pagetable, uint64 va);
pagetable_t uvmcreate(void);
int uvmcopy(pagetable_t old, pagetable_t new, uint64 sz);
void uvmunmap(pagetable_t pagetable, uint64 va, uint64 sz, int do_free);
int copyout(pagetable_t pagetable, uint64 dstva, char *src, uint64 len);
int copyin(pagetable_t pagetable, char *dst, uint64 srcva, uint64 len);
int copyinstr(pagetable_t pagetable, char *dst, uint64 srcva, uint64 max);
/* ======================================================
 * Lab4 新增：启动初始化
 * 文件：kernel/boot/start.c
 * ====================================================== */
void start(void);
void timerinit(void);

/* ======================================================
 * Lab4 新增：中断 / 陷阱处理
 * 文件：kernel/trap/trap.c
 * ====================================================== */
void trapinithart(void);
void kerneltrap(void);
void usertrap(void);
void usertrapret(void);

/* ======================================================
 * Lab5 新增：进程管理
 * 文件：kernel/proc/proc.c
 * ====================================================== */
void procinit(void);
struct proc *myproc(void);
struct cpu *mycpu(void);
int allocpid(void);
struct proc *allocproc(void);
void scheduler(void) __attribute__((noreturn));
void yield(void);
void sched(void);
void sleep(void *chan);
void wakeup(void *chan);
int userinit(void);
int kfork(void);
int kwait(uint64);
int kexec(char *program_name);
/* ======================================================
 * Lab5 新增：上下文切换汇编
 * 文件：kernel/proc/swtch.S
 * ====================================================== */
void swtch(struct context *old, struct context *new);

/* ======================================================
 * Lab6 新增：系统调用分发
 * 文件：kernel/syscall/syscall.c
 * ====================================================== */
void syscall(void);
void argint(int n, int *ip);
void argaddr(int n, uint64 *ip);
void argstr(int n, char *buf, int max);
/* ======================================================
 * Lab6 新增：系统调用具体实现
 * 文件：kernel/syscall/sysproc.c
 * ====================================================== */
uint64 sys_getpid(void);
uint64 sys_exit(void);
uint64 sys_fork(void);
uint64 sys_wait(void);
uint64 sys_sbrk(void);
uint64 sys_write(void);
uint64 sys_yield(void);
uint64 sys_exec(void);
uint64 sys_pipe(void);
uint64 sys_read(void);

uint64 sys_print0(void);
/* ======================================================
 * Lab7 新增：块缓冲层
 * 文件：kernel/fs/bio.c
 * ====================================================== */
void binit(void);
struct buf *bread(uint dev, uint blockno);
void bwrite(struct buf *b);
void brelse(struct buf *b);

/* ======================================================
 * Lab7 新增：文件系统核心
 * 文件：kernel/fs/fs.c
 * ====================================================== */
void fsinit(int dev);
struct inode *iget(uint dev, uint inum);
struct inode *dirlookup(struct inode *dp, char *name, uint *poff);
int readi(struct inode *ip, int user_dst, uint64 dst, uint off, uint n);
int writei(struct inode *ip, int user_src, uint64 src, uint off, uint n);
uint balloc(uint dev);
int pipealloc(struct file **f0, struct file **f1);
int fdalloc(struct file *f);
struct file* filealloc(void);
void fileclose(struct file *f);
int filewrite(int fd, uint64 src, int len);
int fileread(int fd, uint64 dst, int len);
int pipewrite(struct pipe *pi, uint64 src, int n);
int piperead(struct pipe *pi, uint64 dst, int n);
struct user_program {
    const char *name;
    uint64 func;
    uint64 size;
};
extern struct user_program user_programs[];
extern uint64 nuser_programs;

#endif /* DEFS_H */
