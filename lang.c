#pragma once
#include "libc.c"

typedef struct Val {
  char t;
} Val;

typedef struct Sym {
  char t;
  char *s;
} Sym;

typedef struct Pair {
  void *a, *b;
} Pair;

typedef Val *(*FnRef)(Val *, Val **);
typedef struct Fn {
  char t;
  FnRef f;
} Fn;

enum { NIL = 0, PAIR = 2, SYM = 1, FN = 3 };

Val *env = 0;

void print(char *s) {
  while (*s)
    putchar(*s++);
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

char *edit_line(void) {
  int i = 0;
  int size = 0;
  char c, *line = 0;
  for (;;) {
    if (i + 2 >= size) {
      realloc(line, size += 128);
    }
    c = getchar();
    if (c == '\b' || c == 0x7F) {
      if (i > 0) {
        line[i--] = 0;
        print("\b \b");
      }
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

Val *symn(char *s, int n) {
  Sym *p = malloc(sizeof(Sym));
  p->t = 1;
  p->s = strndup(s, n);
  return (Val *)p;
}

Val *sym(char *s) {
  return symn(s, strlen(s));
}

Val *pair(void *a, void *b) {
  Pair *p = malloc(sizeof(Pair));
  p->a = a;
  p->b = b;
  return (Val *)p;
}

Val *fn(FnRef f) {
  Fn *p = malloc(sizeof(Fn));
  p->t = FN;
  p->f = f;
  return (Val *)p;
}

char type(Val *p) { return p ? (p->t & 1) ? p->t : PAIR : NIL; }

Val *head(Val *p) { return type(p) == PAIR ? ((Pair *)p)->a : 0; }

Val *tail(Val *p) { return type(p) == PAIR ? ((Pair *)p)->b : 0; }

char *value(Val *p) { return type(p) == SYM ? ((Sym *)p)->s : 0; }

Val *lookup(Val *env, Val *key) {
  for (; env; env = tail(env)) {
    if (strcmp(value(head(head(env))), value(key)) == 0)
      return head(env);
  }
  return 0;
}

Val *push(Val **list, Val *x, Val **last) {
  if (head(*list)) {
    ((Pair *)*last)->b = pair(x, 0);
    *last = tail(*last);
  } else {
    *list = pair(x, 0);
    *last = *list;
  }
  return *list;
}

Val *parse(char **s) {
  Val *x = 0, *last;
  char *t, *u;
  while (**s && **s <= ' ')
    (*s)++;
  if (**s == '(') {
    (*s)++;
    while (**s && **s != ')')
      push(&x, parse(s), &last);
    return **s == ')' ? (*s)++, x : x;
  }
  if (**s == '"') {
    for (t = u = ++(*s); **s && **s != '"';)
      *u++ = **s == '\\' ? (*s)++, *(*s)++ : *(*s)++;
    return **s == '"' ? (*s)++, symn(t, u - t) : 0;
  }
  for (t = *s; **s > ' ' && **s != ')';)
    (*s)++;
  return *s > t ? symn(t, *s - t) : 0;
}

void print_val(Val *p) {
  char *s;
  switch (type(p)) {
  case NIL:
    print("()");
    break;
  case PAIR:
    putchar('(');
    for (; p; p = tail(p)) {
      print_val(head(p));
      if (tail(p))
        putchar(' ');
    }
    putchar(')');
    break;
  case SYM:
    putchar('"');
    for (s = value(p); *s; s++) {
      if (*s == '"' || *s == '\\')
        putchar('\\');
      putchar(*s);
    }
    putchar('"');
    break;
  default:
    putchar('#');
    print_hex((char *)p, sizeof(void *));
    putchar(' ');
    putchar('#');
    print_hex((char *)p + 4, sizeof(void *));
    break;
  }
}

Val *eval(Val *e, Val **env) {
  Val *x, *y = 0, *z, *last;
  switch (type(e)) {
  case PAIR:
    x = eval(head(e), env);
    if (type(x) != FN) {
      return 0;
    }
    for (z = tail(e); z; z = tail(z)) {
      push(&y, head(z), &last);
    }
    return ((Fn *)x)->f(y, env);
  case SYM:
    x = lookup(*env, e);
    return tail(x);
  default:
    return e;
  }
}

Val *lang(Val *args, Val **_env) {
  char *line, *c;
  Val *x, *env = 0, *last;
  push(&env, pair(sym("meow"), sym("_^..^_")), &last);
  for (;;) {
    x = 0;
    putchar(':');
    putchar(' ');
    line = edit_line();
    putchar('\n');
    c = line;
    if (strcmp(line, "exit") == 0) {
      return 0;
    }
    x = eval(parse(&c), &env);
    print_val(x);
    putchar('\n');
    free(line);
  }
  return 0;
}
