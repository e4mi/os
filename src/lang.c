/* Custom Lisp-like language */
#pragma once
#include "./libc.c"
#include "./nice.c"

/* clang-format off */
typedef void*(*Fn)(void*, void**);
typedef struct { char t; void *a, *b; } Pair;
typedef struct { char t; int n; } Num;
typedef struct { char t; char *s; } Sym;
typedef struct { char t; char *s; } Str; 
typedef struct { char t; Fn f; } Prim;
/* clang-format on */

enum { NIL, PAIR, NUM, SYM, STR, PRIM };

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

void *str(char *s, int n) {
  Str *x = malloc(sizeof(Str));
  x->t = STR;
  x->s = strndup(s, n);
  return (void *)x;
}

void *prim(Fn f) {
  Prim *x = malloc(sizeof(Prim));
  x->t = PRIM;
  x->f = f;
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

void *parse(char **p) {
  char *q;
  void *v = 0, *z;
  int n = 0, sign = 1;

  while (**p && **p <= ' ') {
    (*p)++;
  }
  if (**p == '(') {
    (*p)++;
    v = cons(parse(p), 0);
    z = v;
    while (head(z)) {
      setTail(z, cons(parse(p), 0));
      z = tail(z);
    }
    return v;
  } else if (**p == ')') {
    (*p)++;
    return 0;
  } else if ((**p >= '0' && **p <= '9') || **p == '-') {
    if (**p == '-') {
      sign = -1;
      (*p)++;
    }
    for (q = *p; **p >= '0' && **p <= '9'; (*p)++)
      n = n * 10 + (**p - '0');
    return num(n * sign);
  } else if (**p == '"') {
    (*p)++;
    for (q = *p; **p && **p != '"';) {
      (*p)++;
    }
    if (!**p) {
      return 0;
    }
    return str(q, *(p++) - q);
  } else if (**p) {
    for (q = *p; **p > ' ' && **p != ')' && **p != '('; (*p)++)
      ;
    return *p > q ? sym(q, *p - q) : 0;
  }
  return v;
}

void printValue(void *v) {
  char *p;
  if (!v) {
    print("nil");
    return;
  }
  if (type(v) == PAIR) {
    print("(");
    printValue(head(v));
    for (v = tail(v); head(v); v = tail(v)) {
      print(" ");
      printValue(head(v));
    }
    if (v && type(v) != PAIR) {
      print(" . ");
      printValue(v);
    }
    print(")");
  } else if (type(v) == NUM) {
    printd(((Num *)v)->n);
  } else if (type(v) == SYM) {
    print(((Sym *)v)->s);
  } else if (type(v) == STR) {
    print("\"");
    p = ((Str *)v)->s;
    for (p = ((Str *)v)->s; *p; p++) {
      if (*p == '"' || *p == '\\') {
        print("\\");
      }
      putchar(*p);
    }
    print("\"");
  }
}

void* eval(void *x, void **env) {
  void *v = 0, *w, *z;
  if (type(x) == PAIR) {
    void *a = eval(head(x), env);
    w = tail(x);
    v = cons(eval(head(w), env), 0);
    z = v;
    while (head(w)) {
      setTail(z, cons(eval(head(tail(w)), env), 0));
      z = tail(z);
      w = tail(w);
    }
    if (type(a) == PRIM) {
      return ((Prim *)a)->f(v, env);
    } else {
      return 0;
    }
  } else if (type(x) == SYM) {
    void *v = *env;
    while (head(v)) {
      if (type(head(v)) == PAIR && type(head(head(v))) == SYM) {
        if (strcmp(((Sym *)x)->s, ((Sym *)head(head(v)))->s) == 0) {
          return tail(head(v));
        }
      }
      v = tail(v);
    }
    print(">_< no ");
    printValue(x);
    print("\n");
    return 0;
  } else {
    return x;
  }
}

void *prim_plus(void *arg) {
  int a, b;
  a = type(head(arg)) == NUM ? ((Num *)head(arg))->n : 0;
  b = type(head(tail(arg))) == NUM ? ((Num *)head(tail(arg)))->n : 0;
  return num(a + b);
}

void lang(void) {
  char *line, *p;
  void *x, *env = 0;
  env = cons(cons(sym("+", 1), prim((Fn)prim_plus)), env);
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
    x = eval(x, &env);
    printValue(x);
    print("\n");
    free(line);
  }
}
