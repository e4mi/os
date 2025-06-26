%define RSTACK 1
%define STACK 0
%xdefine stack_state RSTACK
%define rpush push
%define rpop pop
%define rcall call
%define rret ret
%assign _id 0

%macro generate_id 0
  %assign _id _id + 1
%endmacro

%macro use_stack 1
  %ifnidn stack_state, %1
    xchg sp, bp
    %define stack_state %1
  %endif
%endmacro

%macro push 1
  use_stack STACK
  rpush %1
%endmacro

%macro pop 1
  use_stack STACK
  rpop %1
%endmacro

%macro call 1
  use_stack RSTACK
  rcall %1
%endmacro

%macro ret 0
  use_stack RSTACK
  rret
%endmacro

%macro push2 2
  push %2
  push %1
%endmacro

%macro pop2 2
  pop word %1
  pop word %2
%endmacro

%macro read_byte 2
  %ifnidn %1, al
    %error "read_byte al, si"
  %endif
  %ifnidn %2, si
    %error "read_byte al, si"
  %endif
  lodsb
%endmacro

%macro write_byte 2
  %ifnidn %1, si
    %error "write_byte si, al"
  %endif
  %ifnidn %2, al
    %error "write_byte si, al"
  %endif
  stosb
%endmacro

%macro fn 1
  %1:
  %define stack_state RSTACK
%endmacro

%macro endfn 0
%endmacro

%macro test 1
  test %1, %1
%endmacro

%macro while 0
  generate_id
  %xdefine _while_id %[_id]
  .while_%[_id]:
%endmacro

%macro loop 0
  jz .endwhile_%[_id]
%endmacro

%macro endwhile 0
  jmp .while_%[_id]
  .endwhile_%[_id]:
%endmacro

%macro if 2
  generate_id
  %xdefine _if_id %[_id]
  .if_%[_id]:
  cmp %1, %2
  jz .endif_%[_id]
%endmacro

%macro endif 0
  .endif_%[_if_id]:
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
  call main
  call exit
%endmacro

%macro halt 0
  %%halt:
  cli
  hlt
  jmp %%halt
%endmacro

%macro op 1+
  %define _op_code %1
%endmacro

%macro endop 0
  call _op_code
%endmacro

%macro do 1-*
  %define %%op %1
  %rep %0 - 1
    push %2
    %rotate 1
  %endrep
  call %%op
%endmacro