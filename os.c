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

void *memcpy(void *dst, const void *src, size_t n) {
  size_t i;
  for (i = 0; i < n; i++)
    ((char *)dst)[i] = ((char *)src)[i];
  return dst;
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
  size_t m;
  if (!ptr)
    return malloc(n);
  m = malloc_usable_size(ptr);
  if (m >= n)
    return ptr;
  n += 1024;
  r = malloc(n);
  memcpy(r, ptr, m);
  free(ptr);
  return r;
}

ref head(ref x) { return x ? ((ref *)x)[0] : 0; }
ref tail(ref x) { return x ? ((ref *)x)[1] : 0; }
ref set_tail(ref x, ref y) { return ((ref *)x)[1] = y, x; }
ref is_pair(ref x) { return !x || (head(x) & 1) == 0; }

ref mk(ref a, ref b) {
  ref *x = (ref *)malloc(sizeof(ref) * 2);
  x[0] = a;
  x[1] = b;
  return (ref)x;
}

ref push(ref *list, ref x, ref *last) {
  if (head(*list)) {
    set_tail(*last, mk(x, 0));
    *last = tail(*last);
  } else {
    *list = mk(x, 0);
    *last = *list;
  }
  return *list;
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
  t = head(x);
  if (is_pair(x)) {
    print("(");
    while (x) {
      print_cell(head(x));
      if (tail(x)) {
        putchar(' ');
      }
      x = tail(x);
    }
    print(")");
  } else if (t == TXT) {
    print("\"");
    for (s = (char *)tail(x); *s; s++) {
      if (*s == '"' || *s == '\\')
        putchar('\\');
      putchar(*s);
    }
    print("\"");
  } else if (t == NUM) {
    print_dec(tail(x));
  } else {
    print("#"), print_hex((char *)x, sizeof(ref) * 2);
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
ref parse(char **s) {
  char *start, *buffer, *dest;
  ref x = 0, last, item;
  int number = 0, sign = 1;
  while (**s == ' ' || **s == '\t') {
    (*s)++;
  }
  if (**s == '(') {
    (*s)++;
    item = parse(s);
    while (item) {
      push(&x, item, &last);
      item = parse(s);
    }
    return x;
  }
  if (**s == ')') {
    (*s)++;
    return 0;
  }
  if (**s == '-' || (**s >= '0' && **s <= '9')) {
    if (**s == '-') {
      sign = -1;
      (*s)++;
    }
    while (**s >= '0' && **s <= '9') {
      number = number * 10 + (**s - '0');
      (*s)++;
    }
    return mk(NUM, number * sign);
  }
  if (**s == '"') {
    (*s)++;
    start = *s;
    while (**s && **s != '"') {
      if (**s == '\\') {
        (*s)++;
      }
      if (**s) {
        (*s)++;
      }
    }
    if (**s == '"') {
      (*s)++;
    }
    buffer = malloc(*s - start + 1);
    dest = buffer;
    while (*start && *start != '"') {
      if (*start == '\\') {
        start++;
      }
      if (*start) {
        *dest++ = *start++;
      }
    }
    *dest = '\0';
    return mk(TXT, (ref)buffer);
  }
  if (**s > ' ' && **s <= '~' && **s != '(' && **s != ')' && **s != '"') {
    start = *s;
    while (**s > ' ' && **s <= '~' && **s != '(' && **s != ')' && **s != '"') {
      (*s)++;
    }
    return mk(TXT, (ref)memcpy(malloc(*s - start + 1), start, *s - start));
  }
  return 0;
}

ref lookup(char *x, ref env) {
  char *a, *b;
  while (env) {
    a = x;
    b = (char*) head(head(env));
    while (*a && *a == *b) {
      a++, b++;
    }
    if (!*a && !*b) {
      return head(env);
    }
    env = tail(env);
  }
  return 0;
}

ref eval(ref x, ref env) {
  ref op, args, last;
  if (!x) {
    return 0;
  }
  if (is_pair(x)) {
    if (!head(x)) {
      return 0;
    }
    op = eval(head(x), env);
    args = 0;
    while (head(x = tail(x))) {
      push(&args, eval(head(x), env), &last);
    }
    if (head(op) == FN) {
      return ((ref (*)(ref, ref))op)(args, env);
    } else {
      return 0;
    }
  }
  if (head(x) == TXT) {
    return lookup((char *)tail(x), env);
  }
  return x;
}

ref meow(ref args, ref env) {
  print("meow\n");
  return mk(TXT, (ref) "meow");
}

int main(void) {
  char *line = 0, *cursor;
  ref env = 0, x = 0, last = 0;
  push(&env, mk(mk(TXT, (ref) "meow"), mk(FN, (ref)meow)), &last);
  clear();
  print("\n _^..^_ meow!\n\n");
  print_cell(env);
  print("\n");
  while (1) {
    print("> ");
    edit_line(&line);
    print("\n");
    if (same(line, "exit"))
      break;
    print_cell(mk(TXT, (ref)line));
    print("\n");
    cursor = line;
    x = parse(&cursor);
    print_cell(x);
    print("\n");
    x = eval(x, env);
    print_cell(x);
    print("\n");
  }
  print("byeeeee...\n");
  free(line);
  return 0;
}
