#include "./libc.c"
#include "./lang.c"

int main(void) {
  char *line;
  clear();
  puts("\n _^..^_ meow!\n");
  line = malloc(1024);
  while (1) {
    fputs("> ", stdout);
    readline(line, 1024);
    puts("");
    if (strcmp(line, "exit") == 0) {
      break;
    } else if (strcmp(line, "help") == 0) {
      puts("lang, exit, help");
    } else if (strcmp(line, "lang") == 0) {
      lang();
    }
  }
  puts("byeeeee...");
  free(line);
  return 0;
}