#ifndef USER_H
#define USER_H
// system calls
int fork(void);
int exit(void);
int write(int, const void *, int);
int getpid(void);
int print0(void);

// utils
int strlen(const char *s);
void putstr(const char *s);
void putint(int x);
#endif /* USER_H */