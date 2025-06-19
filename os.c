#include "lang.c"

Val *ls(Val *args, Val **env) {
  Val *x = 0, *last;
  push(&x, sym("os.c"), &last);
  push(&x, sym("README.txt"), &last);
  return x;
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

int main(void) {
  char *line, *c;
  Val *x, *env = 0, *last;
  clear();
  push(&env, pair(sym("ls"), fn(ls)), &last);
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
