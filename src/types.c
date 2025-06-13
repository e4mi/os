#pragma once
#include "./lib.c"

enum Type { NIL, NUMBER, TEXT, FUNC, LIST };

typedef void *(*FuncRef)(void*, void **);

typedef struct {
  char type;
} Object;

typedef struct {
  char type;
  int value;
} Number;

typedef struct {
  char type;
  size_t size;
  char *value;
} Text;

typedef struct {
  char type;
  FuncRef function;
} Func;

typedef struct {
  char type;
  size_t size;
  void **items;
} List;

char type(void *v) { return v ? *((char *)v) : NIL; }

void *number(int value) {
  Number *x = malloc(sizeof(Number));
  return x->type = NUMBER, x->value = value, x;
}

void *text(char *value, int len) {
  Text *x = malloc(sizeof(Text));
  return x->type = TEXT, x->value = value, x;
}

void *func(FuncRef function) {
  Func *x = malloc(sizeof(Func));
  return x->type = FUNC, x->function = function, x;
}

void *list(void) {
  List *x = auto_alloc(0, sizeof(List));
  return x->type = LIST, x->size = 0, x->items = 0, x;
}

void *list_push(void **v, void *x) {
  List **a = (List **)v;
  auto_alloc(v, sizeof(List) + ((*a)->size + 1) * sizeof(void *));
  (*a)->items[(*a)->size++] = x;
  return *v;
}

void *list_from(size_t size, void **items) {
  size_t i;
  List *x = list();
  for (i = 0; i < size; i++) {
    list_push((void **)&x, items[i]);
  }
  return x;
}

void printValue(void *v) {
  char *p;
  int i;
  if (!v) {
    print("nil");
    return;
  }
  if (type(v) == LIST) {
    print("(");
    for (i = 0; i < ((List *)v)->size; i++) {
      if (i > 0) {
        print(" ");
      }
      printValue(((List *)v)->items[i]);
    }
    print(")");
  } else if (type(v) == NUMBER) {
    printd(((Number *)v)->value);
  } else if (type(v) == TEXT) {
    for (p = ((Text *)v)->value; *p; p++) {
      if (!((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z') ||
            (*p >= '0' && *p <= '9') || (*p == '_'))) {
        print("\"");
        p = ((Text *)v)->value;
        for (p = ((Text *)v)->value; *p; p++) {
          if (*p == '"' || *p == '\\') {
            print("\\");
          }
          putchar(*p);
        }
        print("\"");
        return;
      }
    }
    print(p);
  }
}
