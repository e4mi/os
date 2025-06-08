asm("jmp _start");

static char *HERE = (char *)0x10000;
static char *MEMORY = (char *)0x10000;
static char *MEMORY_END = (char *)0x9FBFF;
static char PEEK;

void *Alloc(int n);
void Clear(void);
void Emit(char c);
void Exit(int code);
char Key(void);
void Print(char *s);
void PrintNumber(int n);

extern int main(void);
void _start(void) { Exit(main()); }

void *Alloc(int n) {
  char *r = HERE;
  HERE += (n + 3) & ~3;
  if (HERE >= MEMORY_END) {
    Print("\nMEMORY FULL\n");
    Exit(1);
  }
  return r;
}

void Clear(void) {
  asm("int $0x10" : : "a"(0x600), "b"(0x0700), "c"(0), "d"(0x184f));
  asm("int $0x10" : : "a"(0x200), "b"(0), "d"(0));
}

void Emit(char c) {
  if (c == '\n') {
    Emit('\r');
  }
  asm("int $0x10" : : "a"(0x0e00 | c));
}

void Exit(int code) {
  for (;;) {
    asm("hlt");
  }
}

char Key(void) {
  char c;
  asm("mov $0, %%ah; int $0x16" : "=a"(c) : : "memory");
  if (c == '\r') {
    c = '\n';
  }
  return c;
}

void Print(char *s) {
  while (*s) {
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
    } else {
      buf[i++] = c;
      Emit(c);
    }
  }
  buf[i] = 0;
}
