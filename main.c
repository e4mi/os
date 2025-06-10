#include "./libc.c"
#include "./lang.c"

int main(void) {
  char *line;
  clear();
  printf("\n  meow ^.^\n\n> ");
  line = malloc(1024);
  while (1) {
    readline(line, 1024);
    printf("\n> ");
  }
  free(line);
  return 0;
}