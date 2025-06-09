__attribute__((section(".boot"))) void bootloader() {
  __asm__(
      /* setup stack */
      "cli\n"
      "xor %ax, %ax\n"
      "mov %ax, %ds\n"
      "mov %ax, %es\n"
      "mov %ax, %ss\n"
      "mov _org, %sp\n"
      "sti\n"

      /* load from floppy */
      "mov $0x07E0, %ax\n"
      "mov %ax, %es\n"
      "mov $0x0200 + _size, %ax\n"
      "mov $0x0002, %cx\n"
      "xor %dx, %dx\n"
      "xor %bx, %bx\n"
      "int $0x13\n"
      "jc .end\n"

      /* start */
      "jmp Lisp\n"

      /* halt on error */
      ".end:\n"
      "hlt\n"
      "jmp .end\n");
}

#define SECTOR_SIZE 512
#define JUMP(x) goto *(void *)(x - (void *)0x7e00)

typedef unsigned char Byte;
typedef unsigned short Word;
typedef struct {
  char name[64];
  Word firstSector, nextSector;
  unsigned int size;
} FileEntry;
typedef struct Cmd {
  char *name;
  int (*func)(int argc, char **args);
} Cmd;
typedef enum { NIL, PAIR, NUM, SYM, FN, PRIM } Type;
typedef struct {
  Type t;
} Any;
typedef struct {
  Type t;
  Any *a;
  Any *b;
} Pair;
typedef struct {
  Type t;
  int n;
} Num;
typedef struct {
  Type t;
  char *s;
} Sym;

Byte *FilesTable = 0;
void *_HEAP = (void *)0x10000;
const void *_HEAP_MAX = (void *)0x9FBFF;
Cmd *Commands;

void Emit(char c) {
  if (c == '\n') {
    Emit('\r');
  }
  __asm__("mov %0, %%al; mov $0x0E, %%ah; int $0x10" : : "r"(c));
}

void Exit(int code) {
  for (;;) {
    __asm__("hlt");
  }
}

void *Alloc(int n) {
  void *r;
  const char *p = "\n>_< full!\n";
  n = (n + 3) & ~3;
  if (_HEAP + n + sizeof(int) >= _HEAP_MAX) {
    for (; *p; p++) {
      Emit(*p);
    }
    Exit(1);
  }
  r = _HEAP;
  *((int *)r) = n;
  _HEAP += n + sizeof(int);
  return (void *)((char *)r + sizeof(int));
}

int AllocSize(void *ptr) { return ((int *)ptr)[-1]; }

void Free(void *ptr) { /* TODO: free */
}

void Clear(void) {
  __asm__("mov $0x600, %ax; mov $0x0700, %bx; mov $0, %cx; mov $0x184f, %dx; "
          "int $0x10");
  __asm__("mov $0x200, %ax; mov $0, %bx; mov $0, %dx; int $0x10");
}

char Key(void) {
  char c;
  __asm__("mov $0, %%ah; int $0x16; mov %%al, %0" : "=r"(c)::"memory");
  if (c == '\r') {
    c = '\n';
  }
  return c;
}

void Copy(void *dest, void *src, int n);
void Print(char *s);
void PrintNumber(int n);
void PrintString(char *s);
void ReadLine(char *buf, int size);
void *Realloc(void *ptr, int n);
int StringCompare(char *s1, char *s2);

void Copy(void *dest, void *src, int n) {
  int i;
  for (i = 0; i < n; i++) {
    ((char *)dest)[i] = ((char *)src)[i];
  }
}

void Print(char *s) {
  while (*s) {
    Emit(*s++);
  }
}

void PrintTo(char *s, int n) {
  while (n-- > 0) {
    Emit(*s++);
  }
}

void PrintNumber(int n) {
  if (n < 0) {
    Emit('-');
    n = -n;
  }
  if (n / 10) {
    PrintNumber(n / 10);
  }
  Emit((n % 10) + '0');
}

void PrintString(char *s) {
  int i;
  Emit('"');
  for (i = 0; s[i]; i++) {
    if (s[i] == '"' || s[i] == '\\') {
      Emit('\\');
    }
    Emit(s[i]);
  }
  Emit('"');
}

void ReadLine(char *buf, int size) {
  int i = 0;
  char c;
  while ((c = Key()) > 0 && c != '\n' && i < size - 1) {
    if ((c == '\b' || c == 0x7F)) {
      if (i > 0) {
        i--;
        Emit('\b');
        Emit(' ');
        Emit('\b');
      }
    } else if (c >= ' ' && c <= '~') {
      buf[i++] = c;
      Emit(c);
    }
  }
  buf[i] = 0;
}

void *Realloc(void *ptr, int n) {
  void *r;
  int copySize = AllocSize(ptr);
  if (copySize >= n)
    copySize = n;
  if (!ptr)
    return Alloc(n);
  r = Alloc(n);
  if (!r)
    return 0;
  Copy(r, ptr, copySize);
  Free(ptr);
  return r;
}

int StringCompare(char *s1, char *s2) {
  while (*s1 && *s2) {
    if (*s1 != *s2)
      return *s1 - *s2;
    s1++;
    s2++;
  }
  return 0;
}

void ReadSector(unsigned int lba, Byte *buffer) {
  __asm__ volatile("int $0x13" ::"a"(0x02), "b"(0x01), "c"(lba / 18),
                   "d"((lba % 18) + 1), "D"(buffer));
}
void WriteSector(unsigned int lba, Byte *buffer) {
  __asm__ volatile("int $0x13" ::"a"(0x03), "b"(0x01), "c"(lba / 18),
                   "d"((lba % 18) + 1), "D"(buffer));
}

void LoadTable(void) {
  if (!FilesTable) {
    FilesTable = Alloc(SECTOR_SIZE);
    ReadSector(2, FilesTable);
  }
}

Word FreeSector(void) {
  for (Word i = 3; i < 4096; i++)
    if (!FilesTable[i])
      return i;
  return 0xFFFF;
}

Word FreeDirSector(void) {
  Byte buffer[SECTOR_SIZE] = {0};
  for (Word sector = 1; sector != 0xFFFF;) {
    ReadSector(sector, buffer);
    FileEntry *entry = (FileEntry *)buffer;
    if (entry->nextSector == 0xFFFF)
      return entry->nextSector = FreeSector(), WriteSector(sector, buffer),
             FreeSector();
    sector = entry->nextSector;
  }
  return FreeSector();
}

void FileList(void) {
  Byte sector[SECTOR_SIZE] = {0};
  LoadTable();
  for (Word dir_sector = 1; dir_sector != 0xFFFF;) {
    ReadSector(dir_sector, sector);
    FileEntry *entry = (FileEntry *)sector;
    for (int i = 0; i < SECTOR_SIZE / sizeof(FileEntry) && entry[i].name[0];
         i++)
      Print(entry[i].name), Emit('\n');
    dir_sector = entry->nextSector;
  }
}

int FileRead(char *name, Byte *buffer) {
  Byte sector[SECTOR_SIZE] = {0};
  LoadTable();
  for (Word dir_sector = 1; dir_sector != 0xFFFF;) {
    ReadSector(dir_sector, sector);
    FileEntry *entry = (FileEntry *)sector;
    for (int i = 0; i < SECTOR_SIZE / sizeof(FileEntry); i++)
      if (StringCompare(entry[i].name, name) == 0) {
        for (Word s = entry[i].firstSector; s != 0xFFFF; s = FilesTable[s])
          ReadSector(s, buffer), buffer += SECTOR_SIZE;
        return entry[i].size;
      }
    dir_sector = entry->nextSector;
  }
  return -1;
}

int FileWrite(char *name, Byte *buffer, unsigned int size) {
  Byte sector[SECTOR_SIZE] = {0};
  FileEntry *entry = (FileEntry *)sector;
  LoadTable();
  ReadSector(FreeDirSector(), sector);

  for (int i = 0; i < SECTOR_SIZE / sizeof(FileEntry); i++)
    if (!entry[i].name[0]) {
      Copy(entry[i].name, name, 64);
      entry[i].firstSector = FreeSector(), entry[i].size = size,
      entry[i].nextSector = 0xFFFF;
      WriteSector(FreeDirSector(), sector);
      break;
    }

  for (Word s = entry->firstSector; size > 0; s = FreeSector()) {
    WriteSector(s, buffer), buffer += SECTOR_SIZE, size -= SECTOR_SIZE;
    FilesTable[s] = FreeSector();
  }
  FilesTable[FreeSector()] = (Byte)0xFFFF, WriteSector(2, FilesTable);
  return 0;
}

int IsSpace(char c) { return c == ' ' || c == '\t'; }

char **ReadArgs(void) {
  int argv_cap = 8;
  int argc = 0;
  char **argv = Alloc(argv_cap * sizeof(char *));
  if (!argv)
    return 0;

  int c, quoted, word_cap, word_len;
  char *word, *tmp_word;
  char **tmp_argv;

  while (1) {
    /* skip spaces/tabs but stop at newline or 0 */
    do {
      c = Key();
      if (c == 0 || c == '\n')
        break;
    } while (IsSpace((Byte)c));
    if (c == 0)
      Exit(0);
    if (c == '\n')
      break;

    /* determine if token is quoted */
    if (c == '"') {
      quoted = 1;
      c = Key();
    } else {
      quoted = 0;
    }

    /* build the word */
    word_cap = 16;
    word_len = 0;
    word = Alloc(word_cap);
    if (!word)
      goto err;

    while (c != -1) {
      if (quoted && c == '"')
        break;
      if (!quoted && (IsSpace((Byte)c) || c == '\n'))
        break;

      if (c == '\\') {
        c = Key();
        c = c == 'n'    ? '\n'
            : c == 't'  ? '\t'
            : c == '\\' ? '\\'
            : c == '"'  ? '"'
                        : Key();
      }

      if (word_len + 1 >= word_cap) {
        word_cap *= 2;
        tmp_word = Realloc(word, word_cap);
        if (!tmp_word) {
          Free(word);
          goto err;
        }
        word = tmp_word;
      }

      word[word_len++] = (char)c;
      c = Key();
    }
    word[word_len] = '\0';

    /* append word to argv */
    if (argc == argv_cap) {
      argv_cap *= 2;
      tmp_argv = Realloc(argv, argv_cap * sizeof(char *));
      if (!tmp_argv) {
        Free(word);
        goto err;
      }
      argv = tmp_argv;
    }
    argv[argc++] = word;

    /* if unquoted and newline or 0, we're done */
    if (c == 0 || (!quoted && c == '\n'))
      break;
  }

  /* null-terminate the array */
  tmp_argv = Realloc(argv, (argc + 1) * sizeof(char *));
  if (!tmp_argv)
    goto err;
  argv = tmp_argv;
  argv[argc] = 0;
  return argv;

err:
  while (argc--) {
    Free(argv[argc]);
  }
  Free(argv);
  return 0;
}

int FreeArgs(char **argv) {
  if (!argv)
    return 0;
  char **p = argv;
  while (*p)
    Free(*p++);
  Free(argv);
  return 0;
}

int main() {
  Commands = (Cmd[]){{0}};

  Print("\n meow ^.^\n\n");

  for (;;) {
    char found = 0;
    Print("> ");
    char **args = ReadArgs();
    int argc = 0;
    while (args[argc])
      argc++;
    if (!args || !args[0]) {
      FreeArgs(args);
      Print("\n");
      continue;
    }

    // debug
    for (int i = 0; args[i]; i++) {
      PrintString(args[i]);
      Print(" ");
    }
    Print("\n");

    for (int i = 0; Commands[i].name; i++) {
      if (StringCompare(args[0], Commands[i].name) == 0) {
        int status = Commands[i].func(argc, args);
        FreeArgs(args);
        found = 1;
        break;
      }
    }

    if (!found) {
      Print("?\n");
    };

    FreeArgs(args);
  }

  Print("bye\n");
  return 0;
}

Sym *Symbol(char *s) {
  Sym *r = (Sym *)Alloc(sizeof(Sym));
  r->t = SYM;
  r->s = s;
  return r;
}

Pair *Cons(Any *a, Any *b) {
  Pair *r = (Pair *)Alloc(sizeof(Pair));
  r->t = PAIR;
  r->a = a;
  r->b = b;
  return r;
}

void Append(Any **list, Any ***last, Any *item) {
  Pair *t = Cons(item, 0);
  if (!*list)
    *list = (Any *)t;
  else
    **last = (Any *)t;
  *last = &((Pair *)t)->b;
}

Any *Eval(Any *x) { return x; /* stub */ }

Any *Head(Any *x) { return x && x->t == PAIR ? ((Pair *)x)->a : 0; }

Any *Tail(Any *x) { return x && x->t == PAIR ? ((Pair *)x)->b : 0; }

Any *Parse(char **c) {
  Any *x = 0;
  Any **last = &x;
  char *s;
  int n;
  Sym *sym;
  while (**c) {
    while (**c && **c <= ' ')
      (*c)++;
    if (**c == '(' && (*c)++) {
      Append(&x, &last, Parse(c));
      continue;
    }
    if (**c == ')' && (*c)++) {
      return x;
    }
    s = Alloc(64 + 1);
    n = 0;
    while (**c > ' ' && **c != ')' && n < 64) {
      s[n++] = **c;
      (*c)++;
    }
    s[n] = 0;
    sym = Symbol(s);
    Append(&x, &last, (Any *)sym);
    Free(s);
  }
  return x;
}

void PrintAny(Any *x) {
  if (!x) {
    Print("()");
  } else if (x->t == PAIR) {
    Print("(");
    PrintAny(((Pair *)x)->a);
    if (((Pair *)x)->b) {
      x = ((Pair *)x)->b;
      while (x && x->t == PAIR) {
        Print(" ");
        PrintAny(((Pair *)x)->a);
        x = ((Pair *)x)->b;
      }
      if (x) {
        Print(" . ");
        PrintAny(x);
      }
    }
    Print(")");
  } else if (x->t == NUM) {
    PrintNumber(((Num *)x)->n);
  } else if (x->t == SYM) {
    Print(((Sym *)x)->s);
  }
}

int Lisp(void) {
  char *line;
  char *c;
  Any *x;
  line = Alloc(1024);
  Clear();
  Print("\n^.^ meow\n\n");
  for (;;) {
    Print("> ");
    ReadLine(line, 1024);
    Print("\n");
    c = line;
    x = Parse(&c);
    while (Head(x)) {
      PrintAny(Head(x));
      Print(" ");
      x = Tail(x);
    }
    Print("\n");
  }
  Free(line);
  return 0;
}

int Forth(void) {
  long stack[32];
  int sp = 32;
  Byte mem[1024];
  int mp = 0;
  void *code[] = {&&quote, "meow ^^\r\n", &&print, &&end};
  void **ip = code;
  JUMP(*ip++);

quote:
  stack[--sp] = (long)*ip++;
  JUMP(*ip++);

print : {
  char *s = (char *)stack[sp++];
  while (*s) {
    Emit(*s++);
  }
}
  JUMP(*ip++);

end:
  return 0;
}
