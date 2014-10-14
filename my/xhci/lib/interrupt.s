bits 64
section .text

global installisr
extern realisr




;_______________________________
wrapper:

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
call realisr

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
installisr:
push rdi
push rax

lea rax,[rel wrapper]
stosw
mov dword [edi],0x8e000008
add edi,4
shr rax,16
stosw
shr rax,16
stosd
xor eax,eax
stosd

pop rax
pop rdi
ret
;_________________________________
