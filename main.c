#include "./libc.c"
#include "./lang.c"

int main(void) {
  char *line;
  clear();
  printf("\n _^..^_ meow!\n\n");
  line = malloc(1024);
  while (1) {
    printf("> ");
    readline(line, 1024);
    printf("\n");
    if (strcmp(line, "exit") == 0) {
      break;
    } else if (strcmp(line, "help") == 0) {
      printf("lang, exit, help\n");
    } else if (strcmp(line, "lang") == 0) {
      lang();
    }
  }
  printf("byeeeee...\n");
  free(line);
  return 0;
}