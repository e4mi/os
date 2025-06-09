#include "../lib/os.c"

typedef unsigned char u8;
typedef enum { NIL, PAIR, NUM, SYM, FN, PRIM } Type;
typedef struct { Type t; } Any;
typedef struct { Type t; Any* a; Any* b; } Pair;
typedef struct { Type t; int n; } Num;
typedef struct { Type t; char *s; } Sym;

static void _Append(Any **list, Any ***last, Any *item);
static Pair* _Cons(Any* a, Any* b);
static Any* _Eval(Any* x);
static Any* _Head(Any* x);
static Any* _Parse(char **c);
static void _PrintAny(Any* x);
static Sym* _Symbol(char *s);
static Any* _Tail(Any* x);

int main() {
  char line[1024];
  char *c = line;
  Any *x;
  OsClear();
  OsPrint("\n^.^ meow\n\n");
  for (;;) {
    OsPrint("> ");
    OsReadLine(line, sizeof(line));
    OsPrint("\n");
    c = line;
    x = _Parse(&c);
    while (_Head(x)) {
      _PrintAny(_Head(x));
      OsPrint(" ");
      x = _Tail(x);
    }
    OsPrint("\n");
  }
  return 0;
}

static void _Append(Any **list, Any ***last, Any *item) {
  Pair* t = _Cons(item, 0);
  if (!*list) *list = (Any*) t;
  else **last = (Any*) t;
  *last = &((Pair*)t)->b;
}

static Pair* _Cons(Any* a, Any* b) {
  Pair* r = (Pair*) OsAlloc(sizeof(Pair));
  r->t = PAIR; r->a = a; r->b = b;
  return r;
}

static Any* _Eval(Any* x) {
  return x; /* stub */
}

static Any* _Head(Any* x) {
  return x && x->t == PAIR ? ((Pair*)x)->a : 0;
}

static Any* _Parse(char **c) {
  Any *x = 0;
  Any **last = &x;
  char *s;
  int n;
  Sym* sym;
  while (**c) {
    while (**c && **c <= ' ') (*c)++;
    if (**c == '(' && (*c)++) {
      _Append(&x, &last, _Parse(c));
      continue;
    }
    if (**c == ')' && (*c)++) {
      return x;
    }
    s = OsAlloc(64 + 1);
    n = 0;
    while (**c > ' ' && **c != ')' && n < 64) {
      s[n++] = **c;
      (*c)++;
    }
    s[n] = 0;
    sym = _Symbol(s);
    _Append(&x, &last, (Any*) sym);
  }
  return x;
}

static void _PrintAny(Any* x) {
  if (!x) {
    OsPrint("()");
  } else if (x->t == PAIR) {
    OsPrint("(");
    _PrintAny(((Pair*)x)->a);
    if (((Pair*)x)->b) {
      x = ((Pair*)x)->b;
      while (x && x->t == PAIR) {
        OsPrint(" ");
        _PrintAny(((Pair*)x)->a);
        x = ((Pair*)x)->b;
      }
      if (x) {
        OsPrint(" . ");
        _PrintAny(x);
      }
    }
    OsPrint(")");
  } else if (x->t == NUM) {
    OsPrintNumber(((Num*)x)->n);
  } else if (x->t == SYM) {
    OsPrint(((Sym*)x)->s);
  }
}

static Sym* _Symbol(char *s) {
  Sym* r = (Sym*) OsAlloc(sizeof(Sym));
  r->t = SYM; r->s = s;
  return r;
}

static Any* _Tail(Any* x) {
  return x && x->t == PAIR ? ((Pair*)x)->b : 0;
}
