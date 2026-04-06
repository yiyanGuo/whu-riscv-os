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