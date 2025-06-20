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

int main(void) {
  char *line, *c;
  Val *x, *env = 0, *last;
  clear();
  push(&fs, pair(sym("42.c"), sym("main(){return 42}\n")), &last);
  push(&fs, pair(sym("README.txt"), sym("meow\n")), &last);

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
