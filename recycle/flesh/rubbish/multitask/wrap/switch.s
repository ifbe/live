[bits 64]
;____________________________________
taskswitch:

.saverunning:
mov [rel save],rsp
mov rsp,[rel running]
add rsp,0xf8
push rbp              ;+0xff0
push rsi
push rdi              ;+0xfe0
push rax
push rcx
push rdx
push rbx              ;+0xfc0
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15              ;+0xf80

mov rsi,[rel save]
mov rdi,[rel running]
cld
movsq
movsq
movsq
movsq
movsq

.changepointer:
lea rax,[rel pcb]
lea rbx,[rax+0x200]
add dword [rel running],0x100
cmp [rel running],rbx
jb .letitgo
mov [rel running],rax

.letitgo:
mov rsp,[rel running]
add rsp,0x80
pop r15              ;+0xf80
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rbx              ;+0xfc0
pop rdx
pop rcx
pop rax
pop rdi              ;+0xfe0
pop rsi
pop rbp              ;+0xff0

mov rsp,[rel running]
.return:
iretq
;____________________________________
save:dq 0
running:dq 0
