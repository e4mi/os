%macro push2 2
  push %2
  push %1
%endmacro

%macro pop2 1
  pop %1
  pop %2
%endmacro

%macro read_byte 1
  %ifnidn %1, al
    %error "read_byte al, si"
  %endif
  %ifnidn %2, si
    %error "read_byte al, si"
  %endif
  lodsb
%endmacro

%macro write_byte 1
  %ifnidn %1, si
    %error "write_byte si, al"
  %endif
  %ifnidn %2, al
    %error "write_byte si, al"
  %endif
  stosb
%endmacro

%define call_raw call
%macro call 1
  xchg sp, bp
  call_raw %1
  xchg sp, bp
%endmacro

%macro fn 1
  %1:
  xchg sp, bp
%endmacro

%macro endfn 0
  xchg sp, bp
  ret
%endmacro

%macro test 1
  test %1, %1
%endmacro

%macro while 0
  %xdefine _while_id __LINE__
  .while_ %+ _while_id:
%endmacro

%macro do 0
  jz .endwhile_ %+ _while_id
%endmacro

%macro endwhile 0
  jmp .while_ %+ _while_id
  .endwhile_ %+ _while_id:
  %undef _while_id
%endmacro

%macro if 0
  %xdefine _if_id __LINE__
  .if_ %+ _if_id:
%endmacro

%macro then 0
  jz .endif_ %+ _if_id
%endmacro

%macro endif 0
  .endif_ %+ _if_id:
  %undef _if_id
%endmacro

%macro lit 1+
  call %%val
  %%val: db %1
%endmacro

%macro runtime 0
  mov ax, 0
  mov ds, ax
  mov es, ax
  mov ss, ax
  mov sp, 0x7c00
  mov bp, 0x7a00
  call_raw main
  call_raw exit
%endmacro

%macro halt 0
  %%halt:
  cli
  hlt
  jmp %%halt
%endmacro