/* Forth interpreter (WIP) */
#include "../lib/os.c"
#define JUMP(x) goto *(void*) (x - (void*) 0x8000)

typedef unsigned char u8;

int main() {
  long stack[32];
  int sp = 32;
  u8 mem[1024];
  int mp = 0;
  void *code[] = {
    &&quote,
    "meow ^^\r\n",
    &&print,
    &&end
  };
  void **ip = code;
  JUMP(*ip++);

quote:
  stack[--sp] = (long)*ip++;
  JUMP(*ip++);

print:
  {
    char *s = (char*)stack[sp++];
    while (*s) {
      OsEmit(*s++);
    }
  }
  JUMP(*ip++);

end:
  return 0;
}
