#include "../lib/os.c"

int main() {
  char line[1024];
  char *c = line;
  OsClear();
  OsPrint("\n^.^ meow\n\n");
  for (;;) {
    OsPrint("> ");
    OsReadLine(line, sizeof(line));
    OsPrint("\n");
    OsPrint(line);
    OsPrint("\n");
  }
  return 0;
}
