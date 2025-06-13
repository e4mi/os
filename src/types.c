#pragma once
#include "./lib.c"

enum Type { NIL, NUMBER, TEXT, FUNC, PAIR, LIST, MAP };

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

typedef struct {
  char type;
  void *key;
  void *value;
} Pair;

typedef struct {
  char type;
  size_t size;
  Pair **items;
} Map;

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

void *pair(void *key, void *value) {
  Pair *x = malloc(sizeof(Pair));
  return x->type = PAIR, x->key = key, x->value = value, x;
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

void *map(void) {
  Map *x = auto_alloc(0, sizeof(Map));
  return x->type = MAP, x->size = 0, x->items = 0, x;
}

long map_find_index(void **v, void *key) {
  Map **a = (Map **)v;
  long i;
  for (i = 0; i < (*a)->size; i++) {
    if ((*a)->items[i]->key == key) {
      return i;
    }
  }
  return -1;
}

void *map_set(void **v, void *key, void *value) {
  Map **a = (Map **)v;
  long i = map_find_index(v, key);
  if (i >= 0) {
    return (*a)->items[i]->value = value;
  } else {
    auto_alloc(v, sizeof(Map) + ((*a)->size + 1) * sizeof(Pair *));
    (*a)->items[(*a)->size++] = pair(key, value);
  }
  return *v;
}

void *map_get(void **v, void *key) {
  Map **a = (Map **)v;
  long i = map_find_index(v, key);
  if (i >= 0) {
    return (*a)->items[i]->value;
  }
  return 0;
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
