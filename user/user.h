#ifndef USER_H
#define USER_H
// system calls
int fork(void);
int exit(void);
int write(int, const void *, int);
int getpid(void);
int print0(void);
int exec(const char *path, char *argv[]);
int read(int fd, void *buf, int count);
int wait(void);
// // utils
int strlen(const char *s);
int strcmp(const char *s1, const char *s2);
#endif /* USER_H */