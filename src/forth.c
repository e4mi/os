#pragma once
#include "./libc.c"
#include "./nice.c"

void** words = 0;
int wp = 0;
void** env = 0;
int ep = 0;

void* intern(char *s, int len) {
  int i, wl;
  for (i = 0; i < wp; i += wl + sizeof(int)) {
    wl = ((int*)words)[i];
    if (!strncmp(s, (char*)words + i + sizeof(int), wl)) {
      return words + i;
    }
  }
  words = realloc(words, wp + len + sizeof(int));
  ((int*)words)[wp] = len;
  memcpy(words + wp + sizeof(int), s, len);
  wp += len + sizeof(int);
  return words + wp - len - sizeof(int);
}

void* lookup(char *s, int len) {
  int i;
  for (i = wp; --i;) {
    if (strncmp((char*)env[i], s, len) == 0) {
      return env[i];
    }
  }
  return 0;
}

void* defword(char *s, int len, void *f) {
  env = realloc(env, (ep+2)*sizeof(*env));
  env[ep++] = intern(s, len);
  env[ep++] = f;
  return env + ep - 2;
}

void* readword(char **p) {
  char *s;
  for (; **p && **p <= ' '; (*p)++) 
  if (**p == '"') {
    for (s = *p; **p && **p != '"'; (*p)++);
    return strndup(s, *p - s);
  } else if (**p) {
    for (s = *p; **p > ' ' && **p != ')' && **p != '('; (*p)++);
    return lookup(s, *p - s);
  }
}

void forth(void) {
  char *code = "1 2 3 + + prn";
  char *p = code;
  readword(&p);
  env = realloc(env, (ep+1)*sizeof(*env));
  env[ep++] = readword(&p);
}