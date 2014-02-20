%include "interrupt.s"
[BITS 64]
cli
mov esi,0x30000
mov edi,0x100000
mov ecx,0x1000
rep movsq

mov esi,0x20000
mov edi,0x200000
mov ecx,0x1000
rep movsq

xor eax,eax
mov edi,0x80000
mov ecx,0x2000
rep stosq

;process 0
;r15,r14,......r8,rbx,rdx,rcx,rax,rdi,rsi,rbp,rsp
mov byte [0x800f8],0xff       ;running
mov dword [0x80000],death     ;rip
mov byte [0x80008],0x08       ;cs
pushf
pop rax
or rax,0x200
mov qword [0x80010],rax       ;rflag
mov dword [0x80018],0x200000  ;rsp
mov byte [0x80020],0x10       ;ss

;process 1
mov byte [0x801f8],0xff       ;running
mov dword [0x80100],0x100120  ;rip
mov byte [0x80108],0x08       ;cs
pushf
pop rax
or rax,0x200
mov qword [0x80110],rax       ;rflag
mov dword [0x80118],0x200000  ;rsp
mov byte [0x80120],0x10       ;ss

;process 2
mov byte [0x802f8],0xff       ;active
mov dword [0x80200],0x2002d0
mov byte [0x80208],0x08
pushf
pop rax
or rax,0x200
mov qword [0x80210],rax
mov dword [0x80218],0x300000
mov byte [0x80220],0x10

sti

xor rax,rax
death:
mov edi,[0x3028]
mov ecx,4096
rep stosq
inc rax
jmp death

