/* custom lisp-like language */
#pragma once
#include "./libc.c"

typedef struct { int n; char *s; } Str; 
typedef unsigned char u8;
typedef struct { char t; void *a, *b; } Pair;
typedef struct { char t; int n; } Num;
typedef struct { char t; char *s; } Sym;
const char NIL = 0;
const char PAIR = 1;
const char NUM = 2;
const char SYM = 3;
void* cons(void *a, void *b) { Pair* x = malloc(sizeof(Pair)); x->t = PAIR; x->a = a; x->b = b; return (void*)x; }
void* num(int n) { Num* x = malloc(sizeof(Num)); x->t = NUM; x->n = n; return (void*)x; }
void* sym(char *s, int n) { Sym* x = malloc(sizeof(Sym)); x->t = SYM; x->s = strndup(s, n); return (void*)x; }
char type(void *v) { return *((char*) v); }
void* head(void *v) { return type(v) == PAIR ? ((Pair*)v)->a : 0; }
void* tail(void *v) { return type(v) == PAIR ? ((Pair*)v)->b : 0; }
void* setHead(void *v, void *h) { return ((Pair*)v)->a = h; }
void* setTail(void *v, void *t) { return ((Pair*)v)->b = t; }

void* parse(char **s) {
  char *p = *s, *q;
  void *v = 0, *w, *z;
  while (*p && *p <= ' ') p++;
  if (*p == '(') {
    p++;
    w = cons(parse(&p), 0);
    z = w;
    while (head(z)) {
      z = setTail(z, cons(parse(&p), 0));
    }
  } else if (*p == ')') {
    p++;
    return 0;
  } else if (*p == '"') {
    p++;
    for (q = p; *p && *p != '"'; p++);
    if (!*p) return 0;
    return cons(sym("quote", 5), cons(sym(q, p++ - q), 0));
  } else if (*p) {
    for (q = p; *p > ' ' && *p != ')' && *p != '('; p++);
    if (p > q) {
      return sym(q, p - q);
    }
  }
  return v;
}

void printValue(void *v) {
  if (!v) {
    puts("()");
  } else if (type(v) == PAIR) {
    putchar('(');
    printValue(head(v));
    for (v = tail(v); head(v); v = tail(v)) {
      putchar(' ');
      printValue(head(v));
    }
    if (v) {
      fputs(" . ", stdout);
      printValue(v);
    }
    putchar(')');
  } else if (type(v) == NUM) {
    char buf[12];
    itoa(((Num *)v)->n, buf, 10);
    fputs(buf, stdout);
  } else if (type(v) == SYM) {
    fputs(((Sym *)v)->s, stdout);
  }
}

void lang(void) {
  char *line, *p;
  void *x;
  line = malloc(1024);
  putchar('\n');
  while (1) {
    fputs(": ", stdout);
    readline(line, 1024);
    putchar('\n');
    p = line;
    if (strcmp(line, "exit") == 0) {
      putchar('\n');
      break;
    }
    x = parse(&p);
    printValue(x);
  }
  free(line);
}