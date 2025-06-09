#include "../lib/os.c"

typedef struct {
  char *name;
  int (*func)(int argc, char **args);
} Cmd;
static Cmd *Commands;

static int IsSpace(char c) {
  return c == ' ' || c == '\t';
}

static char **ReadArgs(void) {
  int argv_cap = 8;
  int argc = 0;
  char **argv = OsAlloc(argv_cap * sizeof(char *));
  if (!argv) return 0;

  int c, quoted, word_cap, word_len;
  char *word, *tmp_word;
  char **tmp_argv;

  while (1) {
    /* skip spaces/tabs but stop at newline or 0 */
    do {
      c = OsKey();
      if (c == 0 || c == '\n') break;
    } while (IsSpace((unsigned char)c));
    if (c == 0) OsExit(0);
    if (c == '\n') break;

    /* determine if token is quoted */
    if (c == '"') {
      quoted = 1;
      c = OsKey();
    } else {
      quoted = 0;
    }

    /* build the word */
    word_cap = 16;
    word_len = 0;
    word = OsAlloc(word_cap);
    if (!word) goto err;

    while (c != -1) {
      if (quoted && c == '"') break;
      if (!quoted && (IsSpace((unsigned char)c) || c == '\n')) break;

      if (c == '\\') {
        c = OsKey();
        c = c == 'n' ? '\n'
          : c == 't' ? '\t'
          : c == '\\' ? '\\'
          : c == '"' ? '"'
          : OsKey();
      }

      if (word_len + 1 >= word_cap) {
        word_cap *= 2;
        tmp_word = OsRealloc(word, word_cap);
        if (!tmp_word) {
          OsFree(word);
          goto err;
        }
        word = tmp_word;
      }

      word[word_len++] = (char)c;
      c = OsKey();
    }
    word[word_len] = '\0';

    /* append word to argv */
    if (argc == argv_cap) {
      argv_cap *= 2;
      tmp_argv = OsRealloc(argv, argv_cap * sizeof(char *));
      if (!tmp_argv) {
        OsFree(word);
        goto err;
      }
      argv = tmp_argv;
    }
    argv[argc++] = word;

    /* if unquoted and newline or 0, we're done */
    if (c == 0 || (!quoted && c == '\n')) break;
  }

  /* null-terminate the array */
  tmp_argv = OsRealloc(argv, (argc + 1) * sizeof(char *));
  if (!tmp_argv) goto err;
  argv = tmp_argv;
  argv[argc] = 0;
  return argv;

err:
  while (argc--) {
    OsFree(argv[argc]);
  }
  OsFree(argv);
  return 0;
}

static int FreeArgs(char **argv) {
  if (!argv) return 0;
  char **p = argv;
  while (*p) OsFree(*p++);
  OsFree(argv);
  return 0;
}

int main() {
  Commands = (Cmd[]) {
    {0}
  };

  OsPrint("\n meow ^.^\n\n");

  for (;;) {
    char found = 0;
    OsPrint("> ");
    char **args = ReadArgs();
    int argc = 0;
    while (args[argc]) argc++;
    if (!args || !args[0]) {
      FreeArgs(args);
      OsPrint("\n");
      continue;
    }

    // debug
    for (int i = 0; args[i]; i++) {
      OsPrintString(args[i]);
      OsPrint(" ");
    }
    OsPrint("\n");

    for (int i = 0; Commands[i].name; i++) {
      if (OsStringCompare(args[0], Commands[i].name) == 0) {
        int status = Commands[i].func(argc, args);
        FreeArgs(args);
        found = 1;
        break;
      }
    }

    if (!found) {
      OsPrint("?\n");
    };

    FreeArgs(args);
  }

  OsPrint("bye\n");
  return 0;
}
