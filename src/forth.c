#pragma once
#include "./libc.c"
#include "./nice.c"

void **words = 0;
int wp = 0;
void **env = 0;
int ep = 0;
unsigned char **code = 0;
int cp = 0;

/* find or save string to memory, return address of {len, *s} */
void *intern(char *s, int len) {
  int i, wl;
  for (i = 0; i < wp; i += wl + sizeof(int)) {
    wl = ((int *)words)[i];
    if (!strncmp(s, (char *)words + i + sizeof(int), wl)) {
      return words + i;
    }
  }
  words = auto_alloc(words, wp + len + sizeof(int));
  ((int *)words)[wp] = len;
  memcpy(words + wp + sizeof(int), s, len);
  wp += len + sizeof(int);
  return words + wp - len - sizeof(int);
}

/* address of symbol in environment or 0 */
void *lookup(char *s, int len) {
  int i;
  for (i = ep; --i;) {
    if (strncmp((char *)env[i], s, len) == 0) {
      return env[i];
    }
  }
  return 0;
}

/* define word in environment */
void *defword(char *s, int len, void *f) {
  env = auto_alloc(env, (ep + 2) * sizeof(*env));
  env[ep++] = intern(s, len);
  env[ep++] = f;
  return env + ep - 2;
}

void skip_spaces(char **p) {
  for (; **p && **p <= ' ';) {
    (*p)++;
  }
}

int read_digits(char **p) {
  int n = 0;
  skip_spaces(p);
  if (**p == '-') {
    (*p)++;
    return -read_digits(p);
  }
  for (; **p >= '0' && **p <= '9'; (*p)++) {
    n = n * 10 + (**p - '0');
  }
  return n;
}

void *read_word(char **p) {
  char *s;
  skip_spaces(p);
  if (**p == '"') {
    for (s = *p; **p && **p != '"';) {
      (*p)++;
    }
    return strndup(s, *p - s);
  } else if (**p) {
    for (s = *p; **p > ' ' && **p != ')' && **p != '(';) {
      (*p)++;
    }
    return lookup(s, *p - s);
  }
  return 0;
}

unsigned char read_hex(char **p) {
  unsigned char b = 0, i;
  skip_spaces(p);
  for (i = 0; i < 2; i++) {
    b <<= 4;
    if ((**p >= '0' && **p <= '9') || (**p >= 'a' && **p <= 'f')) {
      b = **p > '9' ? (**p - 'a' + 10) : (**p - '0');
    }
    (*p)++;
  }
  return b;
}

void *inlinecode(char **p) {
  void* res = code + cp;
  while (**p) {
    skip_spaces(p);
    if (**p == ')') {
      break;
    }
    code = auto_alloc(code, (cp + 1) * sizeof(unsigned char));
    *code[cp++] = read_hex(p);
  }
  return res;
}

void forth(void) {
  char *code = "1 2 3 + + prn";
  char *p = code;
  while (*p) {
    read_word(&p);
    /* TODO */
  }
}
