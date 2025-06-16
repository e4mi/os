extern void putchar(int);
extern void clear(void);
extern char getchar(void);
extern void exit(int);
extern void *_malloc_here;
extern const void *_malloc_max;

typedef unsigned long size_t;
typedef __SIZE_TYPE__ Ref;
enum { NIL = 0, TXT = 1, NUM = 3, FN = 5 };

void print(char *s) {
  while (*s)
    putchar(*s++);
}

void *memcpy(void *dst, const void *src, size_t n) {
  size_t i;
  for (i = 0; i < n; i++)
    ((char *)dst)[i] = ((char *)src)[i];
  return 0;
}

int strncmp(const char *s1, const char *s2, size_t n) {
  while (n-- > 0 && *s1 && *s1 == *s2)
    s1++, s2++;
  return *s1 - *s2;
}
size_t strlen(const char *s) {
  int i = 0;
  while (s[i])
    i++;
  return i;
}
int strcmp(const char *s1, const char *s2) {
  return strncmp(s1, s2, strlen(s1));
}

void *malloc(size_t n) {
  void *r;
  n = (n + 3) & ~3;
  if ((char *)_malloc_here + n + sizeof(size_t) >= (char *)_malloc_max) {
    print("\n>_< full!\n");
    exit(1);
  }
  r = (void *)((size_t)_malloc_here + sizeof(size_t));
  _malloc_here = (void *)((size_t)_malloc_here + n + sizeof(size_t));
  return r;
}

size_t malloc_usable_size(void *ptr) { return ((size_t *)ptr)[-1]; }

void free(void *ptr) { (void)ptr; }

void *realloc(void *ptr, size_t n) {
  void *r;
  size_t m = malloc_usable_size(ptr);
  if (m >= n)
    return ptr;
  n += 1024;
  if (!ptr)
    return malloc(n);
  r = malloc(n);
  memcpy(r, ptr, m);
  free(ptr);
  return r;
}

Ref get(Ref x, int i) { return ((Ref *)x)[i]; }
Ref mk(Ref a, Ref b) {
  Ref *x = (Ref *)malloc(sizeof(Ref) * 2);
  x[0] = a;
  x[1] = b;
  return (Ref)x;
}

void print_hex(char *x, int n) {
  int i;
  static const char hex[] = "0123456789abcdef";
  for (i = 0; i < n; i++)
    putchar(hex[(x[i] >> 4) & 0xf]), putchar(hex[x[i] & 0xf]);
}

void print_dec(int x) {
  if (x < 0)
    putchar('-'), x = -x;
  if (x >= 10)
    print_dec(x / 10);
  putchar(x % 10 + '0');
}

void print_cell(Ref x) {
  int t;
  char *s;
  if (!x) {
    print("nil");
    return;
  }
  t = get(x, 0);
  if (t == TXT) {
    print("\"");
    for (s = (char *)get(x, 1); *s; s++) {
      if (*s == '"' || *s == '\\')
        putchar('\\');
      putchar(*s);
    }
    print("\"");
  } else if (t == NUM) {
    print_dec(get(x, 1));
  } else {
    print("#"), print_hex((char *)x, sizeof(Ref)*2);
  }
}

/* editable input line */
char *readline(char **line) {
  int i = 0;
  int size = 0;
  char c;
  for (;;) {
    if (i + 2 >= size) {
      realloc(*line, size += 128);
    }
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
int main(void) {
  char *line = 0;
  clear();
  print("\n _^..^_ meow!\n\n");
  print_cell(mk(NUM, 42));
  print("\n");
  print_cell(mk(TXT, (Ref) "meow"));
  print("\n");
  while (1) {
    print("> ");
    readline(&line);
    print("\n");
    if (strcmp(line, "exit") == 0)
      break;
    else
      print_cell(mk(TXT, (Ref) line));
    print("\n");
  }
  print("byeeeee...\n");
  free(line);
  return 0;
}
