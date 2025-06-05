typedef unsigned char B;
typedef unsigned short W;
typedef void F;

W mem[512];
W dp = 0, sp = 512, a, b;
W I(){ return mem[dp++]; }
F O(W a){ mem[--dp] = a; }
F emit(){ asm("int $0x10":"a"(0x0e00 | (I() & 0xff))); }
F key(){ a; asm("int $0x16":"=a"(a)); O(a); }
F eq(){ O(I() == I()); }
F dup(){ a = I(); O(a); O(a); }
F swap(){ a = I(); b = I(); O(a); O(b); }
F cond(){ a = I(); if(I()) asm("jmp *%0"::"m"(a)); }
F sum(){ a = I(), b = I(); O(b + a); }
F read(){ a = I(); O(mem[a]); }
F write(){ a = I(); b = I(); mem[a] = b; }
F alloc(){ a = dp; dp += I(); O(a); }
F exit(){ asm("hlt"); }
F prn(){ a = read(I()); while(mem[a]) { push(mem[a++]); emit(); }}
F parse(){
  a = I();
  b = dp;
  while(read(a++)){
    if (a == ')') {
      O(b);
      return;
    } else if (a == '(') {
      O(a);
      parse();
    } else {
      while (read(a++) !
    }
  }
}

int main(){
  asm("mov %0, %%sp"::"m"(0x7c00));
  return 0;
}