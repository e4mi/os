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

/* auto-growing allocator */
void *auto_alloc(void **ptr, size_t needed_size) {
  size_t new_size;
  size_t current_size = *ptr ? malloc_usable_size(*ptr) : 0;
  if (current_size >= needed_size) return *ptr;
  new_size = (needed_size + 64 - 1) & ~(64 - 1);
  *ptr = realloc(*ptr, new_size);
  return *ptr;
}

/* editable input line */
char* edit_line(char** line) {
  int i = 0;
  char c;
  for (;;) {
    auto_alloc((void**)line, i + 2);
    c = getchar();
    if (c == '\b' || c == 0x7F) {
      if (i > 0) {
        (*line)[i--] = 0;
        putchar('\b');
        putchar(' ');
        putchar('\b');
      }
    }
    if (c >= ' ' && c <= '~') {
      (*line)[i++] = c;
      putchar(c);
    }
    if (c == '\n' || c == 0) {
      (*line)[i] = 0;
      break;
    }
  }
  return *line;
}
