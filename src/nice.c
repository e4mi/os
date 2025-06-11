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

/* editable input line */
char *editline(void) {
  int i = 0;
  int cap = 1024;
  char c;
  char *line = malloc(cap);
  for (;;) {
    c = getchar();
    if (c == '\b' || c == 0x7F) {
      if (i > 0) {
        line[i--] = 0;
        putchar('\b');
        putchar(' ');
        putchar('\b');
      }
    }
    if (i >= cap - 2) {
      cap += 1024;
      line = realloc(line, cap);
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

void *auto_alloc(void *ptr, size_t n) {
  size_t m = malloc_usable_size(ptr);
  if (m >= n) {
    return ptr;
  }
  m = max(n + 1024, m * 2);
  return realloc(ptr, n);
}
