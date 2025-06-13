/* programming language (WIP) */
#pragma once
#include "./lib.c"
#include "./types.c"

static List *words = 0;
static List *env = 0;

Text *intern(char *value, int len) {
  int i;
  Text* word;
  for (i = 0; i < words->size; i++) {
    word = ((Text *)words->items[i]);
    if (word->size == len && strncmp(value, word->value, len) == 0) {
      return word;
    }
  }
  word = text(value, len);
  list_push((void **)&words, word);
  return word;
}

/* address of symbol in environment or 0 */
void *lookup(Text *value) {
  int i;
  for (i = 0; i < env->size; i += 2) {
    if (value == (Text *)env->items[i]) {
      return env->items[i + 1];
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
    v = list();
    z = parse(p);
    while (z) {
      list_push(&v, z);
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
    return number(n * sign);
  } else if (**p == '"') {
    (*p)++;
    for (q = *p; **p && **p != '"';) {
      (*p)++;
    }
    if (!**p) {
      return 0;
    }
    return text(q, *(p++) - q);
  } else if (**p) {
    symbol:
    for (q = *p; **p > ' ' && **p != ')' && **p != '('; (*p)++)
      ;
    return *p > q ? text(q, *p - q) : 0;
  }
  return v;
}

void* eval(void *x, void **env) {
  void *op, *args;
  int i;
  if (type(x) == LIST) {
    if (((List *)x)->size == 0) {
      return 0;
    }
    op = eval(((List *)x)->items[0], env);
    args = list();
    for (i = 1; i < ((List *)x)->size; i++) {
      list_push(&args, eval(((List *)x)->items[i], env));
    }
    if (type(op) == FUNC) {
      return ((Func *)op)->function(args, env);
    } else {
      return 0;
    }
  } else if (type(x) == TEXT) {
    return lookup((Text *)x);
  } else {
    return x;
  }
}

void *prim_meow(void *arg, void **env) {
  return text("meow", 4);
}

void dev(void) {
  char *line = 0, *p;
  void *x;
  env = list();
  words = list();
  list_push((void**)&env, intern("+", 1));
  list_push((void**)&env, func((FuncRef)prim_meow));

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
