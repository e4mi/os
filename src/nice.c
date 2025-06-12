/* nice to have */
#pragma once
#include "./libc.c"

/* print string */
void print(const char *s) {
  char *p;
  for (p = (char *)s; *p; p++) {
    putchar(*p);
  }
}

/* print decimal */
void printd(int n) {
  if (n < 0) {
    putchar('-');
    n = -n;
  }
  if (n / 10) {
    printd(n / 10);
  }
  putchar((n % 10) + '0');
}

void *auto_alloc(void *ptr, size_t n) {
  size_t m = malloc_usable_size(ptr);
  if (m >= n) {
    return ptr;
  }
  m = max(n + 1024, m * 2);
  return realloc(ptr, n);
}


/* editable input line */
void* editline(char* line) {
  int i = 0;
  char c;
  for (;;) {
    line = auto_alloc(line, i + 2);
    c = getchar();
    if (c == '\b' || c == 0x7F) {
      if (i > 0) {
        line[i--] = 0;
        putchar('\b');
        putchar(' ');
        putchar('\b');
      }
    }
    if (c >= ' ' && c <= '~') {
      line[i++] = c;
      putchar(c);
    }
    if (c == '\n' || c == 0) {
      line[i] = 0;
      break;
    }
  }
  return line;
}
