/* subset of libc */
#pragma once

/* from libc_i86.asm */
extern void print(const char *s);
extern void putchar(int c);
extern char getchar(void);
extern void exit(int code);
extern void clear(void);
extern void *_malloc_here;
extern void *_malloc_max;

typedef unsigned char byte_t;
typedef unsigned long size_t;
enum { stdin, stdout, stderr };

int max(int a, int b) {
  return a > b ? a : b;
}

int min(int a, int b) {
  return a < b ? a : b;
}

void fputs(const char *s, ...) {
  while (*s) putchar(*s++);
}

void puts(const char *s) {
  while (*s) putchar(*s++);
  putchar('\n');
}

void* memcpy(void *dst, const void *src, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    ((char *)dst)[i] = ((char *)src)[i];
  }
  return 0;
}

void *malloc(size_t n) {
  void *r;
  n = (n + 3) & ~3;
  if ((char*)_malloc_here + n + sizeof(size_t) >= (char*)_malloc_max) {
    fputs("\n>_< full!\n", stderr);
    exit(1);
  }
  r = _malloc_here;
  *((size_t *)r) = n;
  _malloc_here = (char*)_malloc_here + n + sizeof(size_t);
  return (void *)((char *)r + sizeof(size_t));
}

size_t _malloc_size(void *ptr) {
  return ((size_t *)ptr)[-1];
}

void free(void *ptr) {
  /* TODO */
  (void)ptr;
}

void *realloc(void *ptr, size_t n) {
  void *r;
  size_t m = _malloc_size(ptr);
  if (m >= n) {
    m = n;
  }
  if (!ptr) {
    return malloc(n);
  }
  r = malloc(n);
  memcpy(r, ptr, m);
  free(ptr);
  return r;
}

size_t readline(char *buf, size_t size) {
  size_t i = 0;
  char c;
  while ((c = getchar()) && c != '\n' && i < size - 1) {
    if ((c == '\b' || c == 0x7F)) {
      if (i > 0) {
        i--;
        putchar('\b');
        putchar(' ');
        putchar('\b');
      }
    } else if (c >= ' ' && c <= '~') {
      buf[i++] = c;
      putchar(c);
    }
  }
  buf[i] = 0;
  return i;
}

char* strndup(const char *s, size_t n) {
  char *r = malloc(n + 1);
  memcpy(r, s, n);
  r[n] = 0;
  return r;
}

int strcmp(const char *s1, const char *s2) {
  while (*s1 && *s1 == *s2) s1++, s2++;
  return *s1 - *s2;
}
