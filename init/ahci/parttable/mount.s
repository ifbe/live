bits 64
section .text

extern mount
global mounter


mounter:
mov rdi,0x7070
mov rax,"mount"
stosq
lea rax,[rel mount]
stosq

ret
