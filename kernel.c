#include "io_8086.c"

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
  Clear();
  char line[1024];
  char *c = line;
  Print("meow ^^\n");
  for (;;) {
    Print("> ");
    ReadLine(line, sizeof(line));
    Print("\n");
    c = line;
    Any *x = Parse(&c);
    while (Head(x)) {
      PrintAny(Head(x));
      Print(" ");
      x = Tail(x);
    }
    Print("\n");
  }
  return 0;
}

static void Append(Any **list, Any ***last, Any *item) {
  Pair* t = Cons(item, 0);
  if (!*list) *list = (Any*) t;
  else **last = (Any*) t;
  *last = &((Pair*)t)->b;
}

static Pair* Cons(Any* a, Any* b) {
  Pair* r = (Pair*) Alloc(sizeof(Pair));
  r->t = PAIR; r->a = a; r->b = b;
  return r;
}

static Any* Eval(Any* x) {
  return x; // stub
}

static Any* Head(Any* x) {
  return x && x->t == PAIR ? ((Pair*)x)->a : 0;
}

static Any* Parse(char **c) {
  Any *x = 0;
  Any **last = &x;
  while (**c) {
    while (**c && **c <= ' ') (*c)++;
    if (**c == '(' && (*c)++) {
      Append(&x, &last, Parse(c));
      continue;
    }
    if (**c == ')' && (*c)++) {
      return x;
    }
    char *s = Alloc(64 + 1);
    int n = 0;
    while (**c > ' ' && **c != ')' && n < 64) {
      s[n++] = **c;
      (*c)++;
    }
    s[n] = 0;
    Sym* sym = Symbol(s);
    Append(&x, &last, (Any*) sym);
  }
  return x;
}

static void PrintAny(Any* x) {
  if (!x) {
    Print("()");
  } else if (x->t == PAIR) {
    Print("(");
    PrintAny(((Pair*)x)->a);
    if (((Pair*)x)->b) {
      x = ((Pair*)x)->b;
      while (x && x->t == PAIR) {
        Print(" ");
        PrintAny(((Pair*)x)->a);
        x = ((Pair*)x)->b;
      }
      if (x) {
        Print(" . ");
        PrintAny(x);
      }
    }
    Print(")");
  } else if (x->t == NUM) {
    PrintNumber(((Num*)x)->n);
  } else if (x->t == SYM) {
    Print(((Sym*)x)->s);
  }
}

static Sym* Symbol(char *s) {
  Sym* r = (Sym*) Alloc(sizeof(Sym));
  r->t = SYM; r->s = s;
  return r;
}

static Any* Tail(Any* x) {
  return x && x->t == PAIR ? ((Pair*)x)->b : 0;
}
