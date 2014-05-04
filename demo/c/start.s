bits 64
section .text

extern main

global start
global mymalloc




start:
call main
ret




mymalloc:
add [rel variety1],rdi
mov rax,[rel variety1]
ret
variety1:dq 0x1400000
