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

/* ======================================================
 * 用户态 syscall 测试机器码
 * 说明：
 *   每个数组都可以直接拷贝到用户页起始地址执行。
 *   除 sys_exit 外，其余测试完成后都会停在自旋跳转中。
 * ====================================================== */
static __attribute__((unused)) unsigned char usercode_sys_print0_bin[] = {
    0x93, 0x08, 0x00, 0x00,  /* li a7, 0 */
    0x73, 0x00, 0x00, 0x00,  /* ecall */
    0x6f, 0x00, 0x00, 0x00   /* j . */
};

static __attribute__((unused)) unsigned int usercode_sys_print0_bin_len = sizeof(usercode_sys_print0_bin);

static __attribute__((unused)) unsigned char usercode_sys_getpid_bin[] = {
    0x93, 0x08, 0xb0, 0x00,  /* li a7, 11 */
    0x73, 0x00, 0x00, 0x00,  /* ecall */
    0x6f, 0x00, 0x00, 0x00   /* j . */
};

static __attribute__((unused)) unsigned int usercode_sys_getpid_bin_len = sizeof(usercode_sys_getpid_bin);

static __attribute__((unused)) unsigned char usercode_sys_exit_bin[] = {
    0x13, 0x05, 0x00, 0x00,  /* li a0, 0 */
    0x93, 0x08, 0x20, 0x00,  /* li a7, 2 */
    0x73, 0x00, 0x00, 0x00,  /* ecall */
    0x6f, 0x00, 0x00, 0x00   /* j . */
};

static __attribute__((unused)) unsigned int usercode_sys_exit_bin_len = sizeof(usercode_sys_exit_bin);

static __attribute__((unused)) unsigned char usercode_sys_write_bin[] = {
    0x13, 0x05, 0x10, 0x00,  /* li a0, 1 */
    0x97, 0x05, 0x00, 0x00,  /* auipc a1, 0 */
    0x93, 0x85, 0x85, 0x01,  /* addi a1, a1, 24 */
    0x13, 0x06, 0x60, 0x00,  /* li a2, 6 */
    0x93, 0x08, 0x00, 0x01,  /* li a7, 16 */
    0x73, 0x00, 0x00, 0x00,  /* ecall */
    0x6f, 0x00, 0x00, 0x00,  /* j . */
    0x77, 0x72, 0x69, 0x74, 0x65, 0x0a /* "write\n" */
};

static __attribute__((unused)) unsigned int usercode_sys_write_bin_len = sizeof(usercode_sys_write_bin);

static __attribute__((unused)) unsigned char usercode_pid_loop_bin[] = {
    /* s0 = 5 */
    0x13, 0x04, 0x50, 0x00,

    /* loop: t0 = 8000000，忙等约 1s */
    0xb7, 0x12, 0x7a, 0x00,  0x9b, 0x82, 0x02, 0x20,
    0x93, 0x82, 0xf2, 0xff,  0xe3, 0x9e, 0x02, 0xfe,

    /* write(1, "pid=", 4) */
    0x13, 0x05, 0x10, 0x00,  0x97, 0x05, 0x00, 0x00,
    0x93, 0x85, 0xc5, 0x07,  0x13, 0x06, 0x40, 0x00,
    0x93, 0x08, 0x00, 0x01,  0x73, 0x00, 0x00, 0x00,

    /* getpid() */
    0x93, 0x08, 0xb0, 0x00,  0x73, 0x00, 0x00, 0x00,

    /* a0 += '0' */
    0x13, 0x05, 0x05, 0x03,

    /* pidbuf[0] = a0 */
    0x97, 0x02, 0x00, 0x00,  0x93, 0x82, 0x12, 0x06,
    0x23, 0x80, 0xa2, 0x00,

    /* write(1, pidbuf, 1) */
    0x13, 0x05, 0x10, 0x00,  0x93, 0x85, 0x02, 0x00,
    0x13, 0x06, 0x10, 0x00,  0x93, 0x08, 0x00, 0x01,
    0x73, 0x00, 0x00, 0x00,

    /* write(1, "\n", 1) */
    0x13, 0x05, 0x10, 0x00,  0x97, 0x05, 0x00, 0x00,
    0x93, 0x85, 0xc5, 0x03,  0x13, 0x06, 0x10, 0x00,
    0x93, 0x08, 0x00, 0x01,  0x73, 0x00, 0x00, 0x00,

    /* s0--；打印 5 次后退出 */
    0x13, 0x04, 0xf4, 0xff,  0x63, 0x08, 0x04, 0x00,

    /* yield() */
    0x93, 0x08, 0xd0, 0x00,  0x73, 0x00, 0x00, 0x00,

    /* j loop */
    0x6f, 0xf0, 0x5f, 0xf8,

    /* exit(0) */
    0x13, 0x05, 0x00, 0x00,  0x93, 0x08, 0x20, 0x00,
    0x73, 0x00, 0x00, 0x00,

    /* j . */
    0x6f, 0x00, 0x00, 0x00,

    /* data: "pid=" */
    0x70, 0x69, 0x64, 0x3d,

    /* data: "\n" */
    0x0a,

    /* data: pidbuf */
    0x00
};

static __attribute__((unused)) unsigned int usercode_pid_loop_bin_len = sizeof(usercode_pid_loop_bin);

static __attribute__((unused)) unsigned char usercode_fork_test_bin[] = {
    /* fork() */
    0x93, 0x08, 0x10, 0x00,  0x73, 0x00, 0x00, 0x00,
    0x63, 0x08, 0x05, 0x06,  /* beqz a0, child */

    /* parent: write(1, "parent=", 7) */
    0x13, 0x05, 0x10, 0x00,  0x97, 0x05, 0x00, 0x00,
    0x93, 0x85, 0x45, 0x0d,  0x13, 0x06, 0x70, 0x00,
    0x93, 0x08, 0x00, 0x01,  0x73, 0x00, 0x00, 0x00,

    /* getpid() */
    0x93, 0x08, 0xb0, 0x00,  0x73, 0x00, 0x00, 0x00,
    0x13, 0x05, 0x05, 0x03,  /* a0 += '0' */

    /* pidbuf[0] = a0 */
    0x97, 0x02, 0x00, 0x00,  0x93, 0x82, 0x22, 0x0c,
    0x23, 0x80, 0xa2, 0x00,

    /* write(1, pidbuf, 1) */
    0x13, 0x05, 0x10, 0x00,  0x93, 0x85, 0x02, 0x00,
    0x13, 0x06, 0x10, 0x00,  0x93, 0x08, 0x00, 0x01,
    0x73, 0x00, 0x00, 0x00,

    /* write(1, "\n", 1) */
    0x13, 0x05, 0x10, 0x00,  0x97, 0x05, 0x00, 0x00,
    0x93, 0x85, 0xd5, 0x09,  0x13, 0x06, 0x10, 0x00,
    0x93, 0x08, 0x00, 0x01,  0x73, 0x00, 0x00, 0x00,

    /* exit(0) */
    0x13, 0x05, 0x00, 0x00,  0x93, 0x08, 0x20, 0x00,
    0x73, 0x00, 0x00, 0x00,
    0x6f, 0x00, 0x00, 0x00,  /* j . */

    /* child: write(1, "child=", 6) */
    0x13, 0x05, 0x10, 0x00,  0x97, 0x05, 0x00, 0x00,
    0x93, 0x85, 0xf5, 0x06,  0x13, 0x06, 0x60, 0x00,
    0x93, 0x08, 0x00, 0x01,  0x73, 0x00, 0x00, 0x00,

    /* getpid() */
    0x93, 0x08, 0xb0, 0x00,  0x73, 0x00, 0x00, 0x00,
    0x13, 0x05, 0x05, 0x03,  /* a0 += '0' */

    /* pidbuf[0] = a0 */
    0x97, 0x02, 0x00, 0x00,  0x93, 0x82, 0x62, 0x05,
    0x23, 0x80, 0xa2, 0x00,

    /* write(1, pidbuf, 1) */
    0x13, 0x05, 0x10, 0x00,  0x93, 0x85, 0x02, 0x00,
    0x13, 0x06, 0x10, 0x00,  0x93, 0x08, 0x00, 0x01,
    0x73, 0x00, 0x00, 0x00,

    /* write(1, "\n", 1) */
    0x13, 0x05, 0x10, 0x00,  0x97, 0x05, 0x00, 0x00,
    0x93, 0x85, 0x15, 0x03,  0x13, 0x06, 0x10, 0x00,
    0x93, 0x08, 0x00, 0x01,  0x73, 0x00, 0x00, 0x00,

    /* exit(0) */
    0x13, 0x05, 0x00, 0x00,  0x93, 0x08, 0x20, 0x00,
    0x73, 0x00, 0x00, 0x00,
    0x6f, 0x00, 0x00, 0x00,  /* j . */

    /* data */
    0x70, 0x61, 0x72, 0x65, 0x6e, 0x74, 0x3d, /* "parent=" */
    0x63, 0x68, 0x69, 0x6c, 0x64, 0x3d,       /* "child=" */
    0x0a,                                     /* "\n" */
    0x00, 0x00                                /* pidbuf */
};

static __attribute__((unused)) unsigned int usercode_fork_test_bin_len = sizeof(usercode_fork_test_bin);

#endif /* DEFS_H */
