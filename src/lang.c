/* Custom Lisp-like language */
#pragma once
#include "./libc.c"
#include "./nice.c"

/* clang-format off */
typedef struct { int n; char *s; } Str; 
typedef unsigned char u8;
typedef struct { char t; void *a, *b; } Pair;
typedef struct { char t; int n; } Num;
typedef struct { char t; char *s; } Sym;
/* clang-format on */

const char NIL = 0, PAIR = 1, NUM = 2, SYM = 3;

void *cons(void *a, void *b) {
  Pair *x = malloc(sizeof(Pair));
  x->t = PAIR;
  x->a = a;
  x->b = b;
  return (void *)x;
}

void *num(int n) {
  Num *x = malloc(sizeof(Num));
  x->t = NUM;
  x->n = n;
  return (void *)x;
}

void *sym(char *s, int n) {
  Sym *x = malloc(sizeof(Sym));
  x->t = SYM;
  x->s = strndup(s, n);
  return (void *)x;
}

char type(void *v) { return v ? *((char *)v) : NIL; }
void *head(void *v) { return type(v) == PAIR ? ((Pair *)v)->a : 0; }
void *tail(void *v) { return type(v) == PAIR ? ((Pair *)v)->b : 0; }
void setHead(void *v, void *h) {
  if (type(v) == PAIR)
    ((Pair *)v)->a = h;
}
void setTail(void *v, void *t) {
  if (type(v) == PAIR)
    ((Pair *)v)->b = t;
}

void *parse(char **s) {
  char *p = *s, *q;
  void *v = 0, *z;
  int n = 0, sign = 1;

  while (*p && *p <= ' ')
    p++;
  if (*p == '(') {
    p++;
    v = cons(parse(&p), 0);
    z = v;
    while (head(z)) {
      setTail(z, cons(parse(&p), 0));
      z = tail(z);
    }
    return v;
  } else if (*p == ')') {
    p++;
    return 0;
  } else if ((*p >= '0' && *p <= '9') || *p == '-') {
    if (*p == '-') {
      sign = -1;
      p++;
    }
    for (q = p; *p >= '0' && *p <= '9'; p++)
      n = n * 10 + (*p - '0');
    return num(n * sign);
  } else if (*p == '"') {
    p++;
    for (q = p; *p && *p != '"';) {
      p++;
    }
    if (!*p) {
      return 0;
    }
    return cons(sym("quote", 5), cons(sym(q, p++ - q), 0));
  } else if (*p) {
    for (q = p; *p > ' ' && *p != ')' && *p != '('; p++)
      ;
    return p > q ? sym(q, p - q) : 0;
  }
  return v;
}

void printValue(void *v) {
  if (!v) {
    print("()\n");
    return;
  }
  if (type(v) == PAIR) {
    print("('");
    printValue(head(v));
    for (v = tail(v); head(v); v = tail(v)) {
      print(" ");
      printValue(head(v));
    }
    if (v) {
      print(" . ");
      printValue(v);
    }
    print(")");
  } else if (type(v) == NUM) {
    printd(((Num *)v)->n);
  } else if (type(v) == SYM) {
    print(((Sym *)v)->s);
  }
}

void lang(void) {
  char *line, *p;
  void *x;
  print("\n");
  while (1) {
    print(": ");
    line = editline();
    if (!line) {
      break;
    }
    print("\n");
    p = line;
    if (!strcmp(line, "exit")) {
      free(line);
      break;
    }
    x = parse(&p);
    printValue(x);
    print("\n");
    free(line);
  }
}
