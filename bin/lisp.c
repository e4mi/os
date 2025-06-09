/* Lisp interpreter (WIP) */
#include "../lib/os.c"

typedef unsigned char u8;
typedef enum { NIL, PAIR, NUM, SYM, FN, PRIM } Type;
typedef struct { Type t; } Any;
typedef struct { Type t; Any* a; Any* b; } Pair;
typedef struct { Type t; int n; } Num;
typedef struct { Type t; char *s; } Sym;

static void Append(Any **list, Any ***last, Any *item);
static Pair* Cons(Any* a, Any* b);
static Any* Eval(Any* x);
static Any* Head(Any* x);
static Any* Parse(char **c);
static void PrintAny(Any* x);
static Sym* Symbol(char *s);
static Any* Tail(Any* x);

int main() {
  char *line;
  char *c;
  Any *x;
  line = OsAlloc(1024);
  OsClear();
  OsPrint("\n^.^ meow\n\n");
  for (;;) {
    OsPrint("> ");
    OsReadLine(line, 1024);
    OsPrint("\n");
    c = line;
    x = Parse(&c);
    while (Head(x)) {
      PrintAny(Head(x));
      OsPrint(" ");
      x = Tail(x);
    }
    OsPrint("\n");
  }
  OsFree(line);
  return 0;
}

static void Append(Any **list, Any ***last, Any *item) {
  Pair* t = Cons(item, 0);
  if (!*list) *list = (Any*) t;
  else **last = (Any*) t;
  *last = &((Pair*)t)->b;
}

static Pair* Cons(Any* a, Any* b) {
  Pair* r = (Pair*) OsAlloc(sizeof(Pair));
  r->t = PAIR; r->a = a; r->b = b;
  return r;
}

static Any* Eval(Any* x) {
  return x; /* stub */
}

static Any* Head(Any* x) {
  return x && x->t == PAIR ? ((Pair*)x)->a : 0;
}

static Any* Parse(char **c) {
  Any *x = 0;
  Any **last = &x;
  char *s;
  int n;
  Sym* sym;
  while (**c) {
    while (**c && **c <= ' ') (*c)++;
    if (**c == '(' && (*c)++) {
      Append(&x, &last, Parse(c));
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
    sym = Symbol(s);
    Append(&x, &last, (Any*) sym);
    OsFree(s);
  }
  return x;
}

static void PrintAny(Any* x) {
  if (!x) {
    OsPrint("()");
  } else if (x->t == PAIR) {
    OsPrint("(");
    PrintAny(((Pair*)x)->a);
    if (((Pair*)x)->b) {
      x = ((Pair*)x)->b;
      while (x && x->t == PAIR) {
        OsPrint(" ");
        PrintAny(((Pair*)x)->a);
        x = ((Pair*)x)->b;
      }
      if (x) {
        OsPrint(" . ");
        PrintAny(x);
      }
    }
    OsPrint(")");
  } else if (x->t == NUM) {
    OsPrintNumber(((Num*)x)->n);
  } else if (x->t == SYM) {
    OsPrint(((Sym*)x)->s);
  }
}

static Sym* Symbol(char *s) {
  Sym* r = (Sym*) OsAlloc(sizeof(Sym));
  r->t = SYM; r->s = s;
  return r;
}

static Any* Tail(Any* x) {
  return x && x->t == PAIR ? ((Pair*)x)->b : 0;
}
