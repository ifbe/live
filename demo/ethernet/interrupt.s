global interrupt
extern realisr22




;__________________________________
interrupt:
push rax
push rdi

lea rax,[rel ethernetisr]
mov edi,0x1220
call isrinstall

mov edi,0xfec00000
mov dword [edi],0x10+17*0x1
mov dword [edi+0x10],0x22
mov dword [edi],0x10+17*0x1+1
mov dword [edi+0x10],0

pop rdi
pop rax
ret
;____________________________________




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




;_______________________________
ethernetisr:

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
call realisr22

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

