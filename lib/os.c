#pragma once
__asm__ ("jmp _start");


#include "os_i86.c"

extern int main(void);
void _start(void) { OsExit(main()); }

void OsCopy(void *dest, void *src, int n);
void OsPrint(char *s);
void OsPrintNumber(int n);
void OsReadLine(char *buf, int size);
int OsStringCompare(char *s1, char *s2);

void OsCopy(void *dest, void *src, int n) {
  int i;
  for (i = 0; i < n; i++) {
    ((char*)dest)[i] = ((char*)src)[i];
  }
}

void OsPrint(char *s) {
  while (*s) {
    OsEmit(*s++);
  }
}

void OsPrintN(char *s, int n) {
  while (n-- > 0) {
    OsEmit(*s++);
  }
}

void OsPrintNumber(int n) {
  if (n < 0) {
    OsEmit('-');
    n = -n;
  }
  if (n / 10) {
    OsPrintNumber(n / 10);
  }
  OsEmit((n % 10) + '0');
}

void OsReadLine(char *buf, int size) {
  int i = 0;
  char c;
  while ((c = OsKey()) > 0 && c != '\n' && i < size - 1) {
    if ((c == '\b' || c == 0x7F)) {
      if (i > 0) {
        i--;
        OsEmit('\b');
        OsEmit(' ');
        OsEmit('\b');
      }
    } else if (c >= ' ' && c <= '~') {
      buf[i++] = c;
      OsEmit(c);
    }
  }
  buf[i] = 0;
}

int OsStringCompare(char *s1, char *s2) {
  while (*s1 && *s2) {
    if (*s1 != *s2) return *s1 - *s2;
    s1++;
    s2++;
  }
  return 0;
}
