bits 64
section .text
global waitevent


waitevent:

.compare:
mov rax,[0xff8]
cmp [0xfe8],rax                 ;       [0xff8]=[0xfe8]?
jne .process

.wait:
hlt				;       sleep
jmp .compare			;	waken up

.process:
mov rax,[0xff8]                 ;       pointer=[0xff8]
mov al,[rax]                    ;       return value=[pointer]
inc qword [0xff8]
cmp qword [0xff8],0xfe0 
jb .return
mov qword [0xff8],0x800

.return:
ret
