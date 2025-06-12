/* io, memory, strings, line editing... */
#pragma once

/* from lib_i86.asm */
extern void putchar(char c);
extern char getchar(void);
extern void exit(int code);
extern void clear(void);
extern void *_malloc_here;
extern void *_malloc_max;

typedef unsigned char byte_t;
typedef unsigned long size_t;
enum { stdin, stdout, stderr };

/* copy memory */
void* memcpy(void *dst, const void *src, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    ((char *)dst)[i] = ((char *)src)[i];
  }
  return 0;
}

void *memset(void *dst, int c, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    ((char *)dst)[i] = c;
  }
  return 0;
}

/* allocate memory */
void *malloc(size_t n) {
  void *r;
  char *err = "\n>_< full!\n";
  n = (n + 3) & ~3;
  if ((char*)_malloc_here + n + sizeof(size_t) >= (char*)_malloc_max) {
    while (*err) { putchar(*err++); }
    exit(1);
  }
  r = _malloc_here;
  *((size_t *)r) = n;
  _malloc_here = (char*)_malloc_here + n + sizeof(size_t);
  return (void *)((char *)r + sizeof(size_t));
}

/* get size of allocated memory */
size_t malloc_usable_size(void *ptr) {
  return ((size_t *)ptr)[-1];
}

/* TODO: free memory */
void free(void *ptr) {
  (void)ptr;
}

/* reallocate memory */
void *realloc(void *ptr, size_t n) {
  void *r;
  size_t m = malloc_usable_size(ptr);
  if (m >= n) {
    return ptr;
  }
  n += 1024;
  if (!ptr) {
    return malloc(n);
  }
  r = malloc(n);
  memcpy(r, ptr, m);
  free(ptr);
  return r;
}

/* duplicate string */
char* strndup(const char *s, size_t n) {
  char *r = malloc(n + 1);
  memcpy(r, s, n);
  r[n] = 0;
  return r;
}

/* compare strings, returns 0 if equal */
int strcmp(const char *s1, const char *s2) {
  while (*s1 && *s1 == *s2) s1++, s2++;
  return *s1 - *s2;
}

/* compare strings up to n chars, returns 0 if equal */
int strncmp(const char *s1, const char *s2, size_t n) {
  while (n-- > 0 && *s1 && *s1 == *s2) s1++, s2++;
  return *s1 - *s2;
}

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
