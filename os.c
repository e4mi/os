extern void putchar(int);
extern void clear(void);
extern char getchar(void);
extern void exit(int);
extern void *_malloc_here;
extern const void *_malloc_max;

typedef unsigned long size_t;
typedef __SIZE_TYPE__ ref;
enum { NIL = 0, TXT = 1, NUM = 3, FN = 5 };

void print(char *s) {
  while (*s)
    putchar(*s++);
}

int same(const char *s1, const char *s2) {
  while (*s1 && *s1 == *s2)
    s1++, s2++;
  return *s1 == *s2;
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

void free(void *ptr) { (void)ptr; /* TODO */ }

void *realloc(void *ptr, size_t n) {
  void *r;
  size_t i, m;
  if (!ptr)
    return malloc(n);
  m = malloc_usable_size(ptr);
  if (m >= n)
    return ptr;
  n += 1024;
  r = malloc(n);
  for (i = 0; i < m; i++)
    ((char *)r)[i] = ((char *)ptr)[i];
  free(ptr);
  return r;
}

ref get(ref x, int i) { return ((ref *)x)[i]; }
ref set(ref x, int i, ref y) { return ((ref *)x)[i] = y, x; }

ref mk(ref a, ref b) {
  ref *x = (ref *)malloc(sizeof(ref) * 2);
  x[0] = a;
  x[1] = b;
  return (ref)x;
}

ref push(ref *list, ref x, ref *last) {
  return *last = get(*list, 0) ? set(*last, 1, mk(x, 0)) : (*list = mk(x, 0)), *list;
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

void print_cell(ref x) {
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
    print("#"), print_hex((char *)x, sizeof(ref)*2);
  }
}

char *edit_line(char **line) {
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

ref meow(ref args, ref env) {
  print("meow\n");
  return mk(TXT, (ref) "meow");
}

int main(void) {
  char *line = 0;
  ref env = 0, x = 0, last = 0;
  push(&env, mk(mk(TXT, (ref) "meow"), mk(FN, (ref)meow)), &last);
  clear();
  print("\n _^..^_ meow!\n\n");
  print_cell(mk(NUM, 42));
  print("\n");
  print_cell(mk(TXT, (ref) "meow"));
  print("\n");
  while (1) {
    print("> ");
    edit_line(&line);
    print("\n");
    if (same(line, "exit"))
      break;
    else
      print_cell(mk(TXT, (ref) line));
    print("\n");
  }
  print("byeeeee...\n");
  free(line);
  return 0;
}
