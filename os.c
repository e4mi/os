#include "lang.c"

Val *fs;

Val *keys(Val *args, Val **env) {
  Val *x = 0, *last;
  for (Val *y = fs; y; y = tail(y)) {
    push(&x, head(head(y)), &last);
  }
  return x;
}

Val *ls(Val *args, Val **env) {
  return keys(fs, env);
}

Val *cat(Val *args, Val **env) {
  return tail(lookup(fs, head(args)));
}

Val *pwd(Val *args, Val **env) {
  return sym("/");
}

Val *help(Val *args, Val **env) {
  Val *x = 0, *y, *last;
  for (y = *env; y; y = tail(y)) {
    if (type(tail(head(y))) != FN)
      continue;
    push(&x, head(head(y)), &last);
  }
  return x;
}

Val *os_exit(Val *args, Val **env) {
  print("bye!\n");
  exit(0);
  return 0;
}

unsigned char hex_to_byte(char *s) {
  unsigned char b = 0;
  for (int i = 0; i < 2; i++) {
    b <<= 4;
    if (s[i] >= '0' && s[i] <= '9') {
      b += s[i] - '0';
    } else if (s[i] >= 'a' && s[i] <= 'f') {
      b += s[i] - 'a' + 10;
    } else if (s[i] >= 'A' && s[i] <= 'F') {
      b += s[i] - 'A' + 10;
    }
  }
  return b;
}

/* TODO */
Val *raw(Val *args, Val **env) {
  Val *x;
  char *code;
  int i = 0, cap = 0;
  for (x = args; x; x = tail(x)) {
    if (cap + 1 > cap) {
      code = realloc(code, (cap += 16));
    }
    code[i++] = hex_to_byte(value(head(x)));
  }
  ((void(*)(void))code)();
  return 0;
}

int main(void) {
  char *line, *c;
  Val *x, *env = 0, *last;
  clear();
  push(&fs, pair(sym("42.c"), sym("main(){return 42}\n")), &last);
  push(&fs, pair(sym("README.txt"), sym("meow\n")), &last);

  push(&env, pair(sym("raw"), fn(raw)), &last);
  push(&env, pair(sym("ls"), fn(ls)), &last);
  push(&env, pair(sym("cat"), fn(cat)), &last);
  push(&env, pair(sym("pwd"), fn(pwd)), &last);
  push(&env, pair(sym("lang"), fn(lang)), &last);
  push(&env, pair(sym("exit"), fn(os_exit)), &last);
  push(&env, pair(sym("help"), fn(help)), &last);
  print("\n _^..^_ meow!\n\n");
  eval(pair(sym("meow"), 0), &env);
  for (;;) {
    x = 0;
    putchar('>');
    putchar(' ');
    line = edit_line();
    putchar('\n');
    c = line;
    while (*c) {
      push(&x, parse(&c), &last);
    }
    if (type(head(x)) == PAIR) {
      x = eval(head(x), &env);
    } else {
      x = eval(x, &env);
    }
    print_val(x);
    putchar('\n');
    free(line);
  }
  return 0;
}
