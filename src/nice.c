/* nice to have */
#pragma once
#include "./libc.c"

/* print string */
void print(const char *s) {
  char *p;
  for (p = (char *)s; *p; p++) putchar(*p);
}

/* editable input line */
char* editline(void) {
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
