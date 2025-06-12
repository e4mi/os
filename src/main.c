#include "./libc.c"
#include "./nice.c"
#include "./lang.c"

int main(void) {
  char *line = 0;
  clear();
  print("\n _^..^_ meow!\n\n");
  while (1) {
    print("> ");
    line = edit_line(line);
    print("\n");
    if (strcmp(line, "exit") == 0) {
      break;
    } else if (strcmp(line, "help") == 0) {
      print("lang, exit, help\n");
    } else if (strcmp(line, "lang") == 0) {
      lang();
    } else {
      print(">_< ");
      print(line);
      print("?\n");
    }
  }
  print("byeeeee...\n");
  free(line);
  return 0;
}
