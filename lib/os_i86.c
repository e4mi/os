#pragma once
static char *_HEAP = (char *)0x10000;
static char *_HEAP_MAX = (char *)0x9FBFF;
static const char *_ERROR = "\n>_< full!\n";

void *OsAlloc(int n);
void OsClear(void);
void OsEmit(char c);
void OsExit(int code);
char OsKey(void);

void *OsAlloc(int n) {
  char *r = _HEAP;
  _HEAP += (n + 3) & ~3;
  if (_HEAP >= _HEAP_MAX) {
    const char *p;
    for (p = _ERROR; *p; p++) {
      OsEmit(*p);
    }
    OsExit(1);
  }
  return r;
}

void OsClear(void) {
  __asm__ ("mov $0x600, %ax; mov $0x0700, %bx; mov $0, %cx; mov $0x184f, %dx; int $0x10");
  __asm__ ("mov $0x200, %ax; mov $0, %bx; mov $0, %dx; int $0x10");
}

void OsEmit(char c) {
  if (c == '\n') {
    OsEmit('\r');
  }
  __asm__ ("mov %0, %%al; mov $0x0E, %%ah; int $0x10" : : "r"(c));
}

void OsExit(int code) {
  for (;;) {
    __asm__ ("hlt");
  }
}

char OsKey(void) {
  char c;
  __asm__ ("mov $0, %%ah; int $0x16; mov %%al, %0" : "=r"(c) :: "memory");
  if (c == '\r') {
    c = '\n';
  }
  return c;
}

