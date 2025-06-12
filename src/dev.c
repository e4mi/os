/* Custom Lisp-like language without cons */
#pragma once
#include "./libc.c"
#include "./nice.c"

/* clang-format off */
typedef void*(*NFn)(void*, void**);
typedef struct { char t; int n; } NNum;
typedef struct { char t; size_t l; char *s; } NSym;
typedef struct { char t; size_t l; char *s; } NStr; 
typedef struct { char t; NFn f; } NPrim;
typedef struct { char t; size_t l; void **i; } NArray;
/* clang-format on */

static const char NIL = 0;
static const char NUM = 2;
static const char SYM = 3;
static const char STR = 4;
static const char PRIM = 5;
static const char ARRAY = 6;

static NArray *words = 0;
static NArray *env = 0;

void *num(int n) {
  NNum *x = malloc(sizeof(NNum));
  x->t = NUM;
  x->n = n;
  return (void *)x;
}

void *sym(char *s, int n) {
  NSym *x = malloc(sizeof(NSym));
  x->t = SYM;
  x->s = strndup(s, n);
  return (void *)x;
}

void *str(char *s, int n) {
  NStr *x = malloc(sizeof(NStr));
  x->t = STR;
  x->s = strndup(s, n);
  return (void *)x;
}

void *prim(NFn f) {
  NPrim *x = malloc(sizeof(NPrim));
  x->t = PRIM;
  x->f = f;
  return (void *)x;
}

void *array(void) {
  NArray *x = auto_alloc(0, sizeof(NArray));
  x->t = ARRAY, x->l = 0, x->i = 0;
  return (void *)x;
}

void array_push(void **v, void *x) {
  NArray **a = (NArray **) v;
  auto_alloc(v, sizeof(NArray) + ((*a)->l + 1) * sizeof(void *));
  (*a)->i[(*a)->l++] = x;
}

char type(void *v) { return v ? *((char *)v) : NIL; }

NSym *intern(char *s, int len) {
  int i;
  NSym* word;
  for (i = 0; i < words->l; i++) {
    word = ((NSym *)words->i[i]);
    if (word->l == len && strncmp(s, word->s, len) == 0) {
      return word;
    }
  }
  word = sym(s, len);
  array_push((void **)&words, word);
  return word;
}

/* address of symbol in environment or 0 */
void *lookup(NSym *s) {
  int i;
  for (i = 0; i < env->l; i += 2) {
    if (s == (NSym *)env->i[i]) {
      return env->i[i + 1];
    }
  }
  return 0;
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
    v = array();
    z = parse(p);
    while (z) {
      array_push(&v, z);
      z = parse(p);
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
    if (q == *p) {
      (*p)--;
      goto symbol;
    }
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
    symbol:
    for (q = *p; **p > ' ' && **p != ')' && **p != '('; (*p)++)
      ;
    return *p > q ? sym(q, *p - q) : 0;
  }
  return v;
}

void printValue(void *v) {
  char *p;
  int i;
  if (!v) {
    print("nil");
    return;
  }
  if (type(v) == ARRAY) {
    print("(");
    for (i = 0; i < ((NArray *)v)->l; i++) {
      if (i > 0) {
        print(" ");
      }
      printValue(((NArray *)v)->i[i]);
    }
    print(")");
  } else if (type(v) == NUM) {
    printd(((NNum *)v)->n);
  } else if (type(v) == SYM) {
    print(((NSym *)v)->s);
  } else if (type(v) == STR) {
    print("\"");
    p = ((NStr *)v)->s;
    for (p = ((NStr *)v)->s; *p; p++) {
      if (*p == '"' || *p == '\\') {
        print("\\");
      }
      putchar(*p);
    }
    print("\"");
  }
}

void* eval(void *x, void **env) {
  void *op, *args;
  int i;
  if (type(x) == ARRAY) {
    if (((NArray *)x)->l == 0) {
      return 0;
    }
    op = eval(((NArray *)x)->i[0], env);
    args = array();
    for (i = 1; i < ((NArray *)x)->l; i++) {
      array_push(&args, eval(((NArray *)x)->i[i], env));
    }
    if (type(op) == PRIM) {
      return ((NPrim *)op)->f(args, env);
    } else {
      return 0;
    }
  } else if (type(x) == SYM) {
    return lookup((NSym *)x);
  } else {
    return x;
  }
}

void *prim_meow(void *arg, void **env) {
  return str("meow", 4);
}

void dev(void) {
  char *line = 0, *p;
  void *x;
  env = array();
  words = array();
  array_push((void**)&env, intern("+", 1));
  array_push((void**)&env, prim((NFn)prim_meow));

  print("\n");
  while (1) {
    print(": ");
    edit_line(&line);
    if (!*line) {
      continue;
    }
    print("\n");
    p = line;
    if (!strcmp(line, "exit")) {
      break;
    }
    x = parse(&p);
    x = eval(x, (void*)&env);
    printValue(x);
    print("\n");
  }
  free(line);
  free(env);
  free(words);
}
