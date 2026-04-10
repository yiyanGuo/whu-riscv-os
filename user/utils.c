// #include "user.h"



// int strlen(const char *s) {
//     int n = 0;
//     while (s[n]) n++;
//     return n;
// }

// void putstr(const char *s) {
//     write(1, s, strlen(s));
// }

// void putint(int x) {
//     char buf[16];
//     int i = 0;

//     if (x == 0) {
//         char c = '0';
//         write(1, &c, 1);
//         return;
//     }

//     if (x < 0) {
//         char c = '-';
//         write(1, &c, 1);
//         x = -x;
//     }

//     while (x > 0) {
//         buf[i++] = '0' + (x % 10);
//         x /= 10;
//     }

//     while (i > 0) {
//         i--;
//         write(1, &buf[i], 1);
//     }
// }