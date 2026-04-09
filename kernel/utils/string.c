#include "types.h"

int memset(char *sa, char val, uint64 size) {
  for(char *p = sa; p < sa + size; p++) {
    *p = val;
  }
  return 0;
}

int memmove(char *dest, char *src, uint64 size) {
  for(int i = 0; i < size; i++) {
    dest[i] = src[i];
  }
  return 0;
}

int strlen(char *str) {
  int len = 0;
  while(*str != '\0')
    len++;
  return len;
}

int streq(const char *str1, const char *str2) {
  while(*str1 != '\0' || *str2 != '\0') {
    if(*str1 != *str2) return 0;
    str1++;
    str2++;
  }
  return 1;
}
