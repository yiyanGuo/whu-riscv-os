#ifndef FILE_H
#define FILE_H

#include "types.h"

struct pipe {
    char data[512];
    uint nread; // 读取位置
    uint nwrite; // 写入位置
    int readopen; // 读端是否打开
    int writeopen; // 写端是否打开
};

struct file {
    enum {FD_NONE, FD_PIPE, FD_INODE} type;
    int ref; // 引用计数
    int readable; // 是否可读
    int writable; // 是否可写
    struct pipe *pipe; // type == FD_PIPE 时有效
    struct inode *inode; // type == FD_INODE 时有效
    uint off; // 文件偏移（type == FD_INODE 时有效）
};

#endif /* FILE_H */