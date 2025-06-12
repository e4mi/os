#include "./lib.c"
#include "./types.c"
#include "./dev.c"

int main(void) {
  char *line = 0;
  clear();
  print("\n _^..^_ meow!\n\n");
  while (1) {
    print("> ");
    edit_line(&line);
    print("\n");
    if (strcmp(line, "exit") == 0) {
      break;
    } else if (strcmp(line, "help") == 0) {
      print("dev, exit, help\n");
    } else if (strcmp(line, "dev") == 0) {
      dev();
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
