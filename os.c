/*--- LIBC ---*/
typedef unsigned long size_t;

/* from os_ARCH.asm */
extern void *_malloc_here;
extern const void *_malloc_max;
extern void putchar(int c);
extern void exit(int code);
extern void clear(void);
extern char getchar(void);

/* print string */
extern void print(const char *s) {
  while (*s) {
    putchar(*s++);
  }
}

/* print bytes as hex numbers */
void print_hex(void *x, int n) {
  char c;
  while (n--) {
    c = ((char *)x)[n - 1];
    putchar((c >> 4 & 0xf) + '0' + ((c >> 4 & 0xf) > 9) * 7);
    putchar((c & 0xf) + '0' + ((c & 0xf) > 9) * 7);
  }
}

/* copy memory */
void *memcpy(void *dst, const void *src, size_t n) {
  size_t i;
  for (i = 0; i < n; i++) {
    ((char *)dst)[i] = ((char *)src)[i];
  }
  return 0;
}

/* allocate memory */
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

/* get size of allocated memory */
size_t malloc_usable_size(void *ptr) { return ((size_t *)ptr)[-1]; }

/* TODO: free memory */
void free(void *ptr) { (void)ptr; }

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

size_t strlen(const char *s) {
  size_t i = 0;
  while (s[i]) {
    i++;
  }
  return i;
}
/* compare strings up to n chars, returns 0 if equal */
int strncmp(const char *s1, const char *s2, size_t n) {
  while (n-- > 0 && *s1 && *s1 == *s2)
    s1++, s2++;
  return *s1 - *s2;
}
int strcmp(const char *s1, const char *s2) {
  return strncmp(s1, s2, strlen(s1));
}
char *strndup(const char *s, size_t n) {
  char *r = malloc(n + 1);
  memcpy(r, s, n);
  r[n] = 0;
  return r;
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

/* --- LANGUAGE --- */
const size_t NIL = 0x0, TXT = 0x1, NUM = 0x3, FN = 0x5;
char is_pair(void *x) { return ((size_t)x & 1) == 0; }

static void *cell(void *a, void *b) {
  void **x = malloc(sizeof(void *) * 2);
  x[0] = a;
  x[1] = b;
  return x;
}
void *head(void *x) { return ((void **)x)[0]; }
void *tail(void *x) { return ((void **)x)[1]; }
size_t raw(void *x) { return (size_t)((void **)x)[1]; }
void *pair(void *a, void *b) { return cell(a, b); }
void *txt(char *s) { return cell((void *)TXT, strndup(s, strlen(s))); }
void *txtn(char *s, size_t n) { return cell((void *)TXT, strndup(s, n)); }
void *num(int i) {
  int *x = malloc(sizeof(int));
  *x = i;
  return cell((void *)NUM, x);
}
void *fn(void *(*f)(void **, void **)) { return cell((void *)FN, (void *)f); }
char type(void *x) { return (!x || is_pair(x)) ? NIL : (size_t)head(x); }

void push(void **z, void *x, void **last) {
  *last = *z ? last[1] = cell(x, 0) : (*z = cell(x, 0));
}

void free_cell(void *x) {
  if (!x)
    return;
  if (type(x) & (TXT | NUM))
    free(((void **)x)[1]);
  free(x);
}

void print_cell(void *x) {
  char *s;
  if (!x) {
    print("nil");
    return;
  }
  if (is_pair(x)) {
    if (!head(x)) {
      print("nil");
      return;
    }
    putchar('(');
    print_cell(head(x));
    while (is_pair(x = tail(x))) {
      putchar(' ');
      print_cell(head(x));
    }
    if (!is_pair(x)) {
      print(" . ");
      print_cell(x);
    }
    putchar(')');
  } else {
    char t = type(x);
    if (t == TXT) {
      for (s = ((char **)x)[1]; *s; s++) {
        if (*s == '"' || *s == '\\') {
          putchar('\\');
        }
        putchar(*s);
      }
    } else {
      print("#");
      print_hex(x, sizeof(void *));
    }
  }
}

void *parse(char **s) {
  char *t;
  int n = 0, sign = 1;
  while (**s == ' ' || **s == '\t') {
    (*s)++;
  }
  if (**s == '-' || (**s >= '0' && **s <= '9')) {
    if (**s == '-') {
      sign = -1;
      (*s)++;
    }
    while (**s >= '0' && **s <= '9') {
      n = n * 10 + (**s - '0');
      (*s)++;
    }
    return num(n * sign);
  }
  if (**s >= ' ' && **s <= '~' && **s != '(' && **s != ')') {
    t = *s;
    while (**s >= ' ' && **s <= '~' && **s != '(' && **s != ')') {
      (*s)++;
    }
    return txtn(t, *s - t);
  } else if (**s == '"') {
    (*s)++;
    t = *s;
    while (**s && **s != '"') {
      if (**s == '\\') {
        (*s)++;
      }
      (*s)++;
    }
    if (**s == '"') {
      (*s)++;
      return txtn(t, *s - t - (size_t)1);
    }
  }
  return 0;
}

void *eval(void *x, void **env) {
  void *op, *args, *last;
  if (!x) {
    return 0;
  } else if (is_pair(x)) {
    op = eval(head(x), env);
    args = 0;
    while (head(x)) {
      push(&args, eval(head(x), env), &last);
    }
    if (type(op) == FN) {
      return ((void *(*)(void **, void **))op)(args, env);
    } else {
      return 0;
    }
  } else if (type(x) == TXT) {
    while (env) {
      if (strncmp(tail(x), tail(head(head(env))), strlen(x)) == 0) {
        return tail(head(env));
      }
      env = tail(env);
    }
    return 0;
  } else {
    return x;
  }
}

/*--- ENV ---*/
void *meow(void **x, void **env) { return print("meow\n"), txt("meow"); }

/*--- MAIN ---*/
int main(void) {
  void *x = 0, *env = 0, *last;
  char *line = 0;
  push(&env, pair(txt("meow"), fn(meow)), &last);
  clear();
  print("\n _^..^_ meow!\n\n");
  while (1) {
    print("> ");
    readline(&line);
    print("\n");
    if (strcmp(line, "exit") == 0) {
      break;
    } else {
      x = parse(&line);
      x = eval(x, env);
      print_cell(x);
      print("\n");
    }
  }
  print("byeeeee...\n");
  free(line);
  return 0;
}
