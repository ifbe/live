bits 64
section .text

global int20
extern realisr20




;________________________________
int20:
push rax
push rdi
	lea rax,[rel isr20]
	mov edi,0x1200
	call isrinstall
pop rdi
pop rax

ret
;_______________________________




;_______________________________
isr20:

;preserve everything
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp

push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15

;real
call realisr20

;eoi
mov rax,0xfee000b0
mov dword [rax],0

;restore everything
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8

pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax

iretq
;_______________________________




;________________________________
isrinstall:
stosw
mov dword [edi],0x8e000008
add edi,4
shr rax,16
stosw
shr rax,16
stosd
xor eax,eax
stosd
ret
;_________________________________
