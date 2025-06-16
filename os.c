extern void putchar(int c);
extern void clear(void);
extern char getchar(void);
extern void exit(int code);
extern void *_malloc_here;
extern const void *_malloc_max;
typedef unsigned long size_t;
#if defined(__x86_64__)
typedef unsigned long uintptr_t;
#else
typedef unsigned int uintptr_t;
#endif

void print(char *s) {
  while (*s) {
    putchar(*s++);
  }
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
typedef struct {
  char type;
} Value;
typedef struct {
  char type;
  void *a, *b;
} Pair;
typedef struct {
  char type;
  int n;
} Num;
typedef struct {
  uintptr_t t;
  char *s;
} Txt;
typedef struct {
  uintptr_t t;
  void *(*f)(void **, void **);
} Fn;

enum { NIL = 0, TXT = 1, NUM = 3, FN = 5 };
typedef uintptr_t Ref;
typedef Ref Cell[2];
Ref get(Ref x, int i) { return (Ref)((void **)x)[i]; }
Ref set(Ref x, Ref a, Ref b) { return ((Ref *)x)[0] = a, ((Ref *)x)[1] = b, x; }
char type(Ref x) { return (get(x, 0) & 1) ? get(x, 0) : NIL; }
Ref mk(Ref a, Ref b) { return set((Ref)malloc(sizeof(Ref) * 2), a, b); }
void print_hex(char *x, int n) {
  int i = 0;
  char *a = "0123456789abcdef";
  for (i = 0; i < n; i++) {
    putchar(a[x[i] >> 4 & 0xf]);
    putchar(a[x[i] & 0xf]);
  }
}
void print_dec(int x) {
  if (x < 0) {
    putchar('-');
    x = -x;
  }
  if (x / 10) {
    print_dec(x / 10);
  }
  putchar(x % 10 + '0');
}
void print_cell(Ref x) {
  char *s;
  if (!x) {
    print("nil");
    return;
  } else if (type(x) == NIL) {
    print("(");
    print_cell(get(x, 0));
    while (!type(x = get(x, 1))) {
      putchar(' ');
      print_cell(get(x, 0));
    }
    if (!type(x)) {
      print(" . ");
      print_cell(x);
    }
    putchar(')');
  } else {
    int t = type(x);
    print("\n");
    if (t == TXT) {
      print("\"");
      for (s = (char *)get(x, 1); *s; s++) {
        if (*s == '"' || *s == '\\') {
          putchar('\\');
        }
        putchar(*s);
      }
      print("\"");
    } else if (t == NUM) {
      print_dec(get(x, 1));
    } else {
      print("#");
      print_hex((char *)x, sizeof(Ref));
      print(" #");
      print_hex((char*)x + sizeof(Ref), sizeof(Ref));
    }
  }
}

int main(void) {
  Ref x = 0;
  clear();
  print("\n _^..^_ meow!\n\n");
  print_cell(mk(NUM, 42));
  print("\n");
  print_cell(mk(TXT, (Ref)"meow"));
  print("\n");
  return 0;
}
