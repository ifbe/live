bits 64
section .text

extern mount
global automount


automount:
mov rdi,0x4000
.findfreespace:
cmp qword [rdi],0
je .startmount
add rdi,0x10
jmp .findfreespace

.startmount:
mov rax,"mount"
stosq
lea rax,[rel mount]
stosq

ret
