%define kbdhome 0x5000
%define bufsize 0x1000
%define idthome 0x3000
[bits 64]


;清空input buffer
    mov edi,kbdhome
    mov ecx,bufsize
    xor rax,rax
    rep stosb

    mov rax,"input"
    mov [kbdhome+bufsize-0x20],rax
    mov rax,kbdhome
    mov [kbdhome+bufsize-0x18],rax
    mov rax,"progress"
    mov [kbdhome+bufsize-0x10],rax
    mov rax,kbdhome
    mov [kbdhome+bufsize-0x8],rax


;_____________int 21h__________________
;keyboard:
cli
lea rax,[rel keyboardisr]
mov edi,idthome+0x21*0x10
call isrinstall

;enable
mov edi,0xfec00000
mov dword [edi],0x10+2*0x1
mov dword [edi+0x10],0x21
mov dword [edi],0x10+2*0x1+1
mov dword [edi+0x10],0

jmp endofkeyboard


keyboardisr:
push rax
push rbx
	in al,0x60
	mov ebx,[kbdhome+bufsize-0x18]			;地址
	mov [ebx],al
	inc ebx
	cmp ebx,kbdhome+bufsize-0x40
	jb .newaddr
	mov ebx,kbdhome
	.newaddr:
	mov [kbdhome+bufsize-0x18],ebx
.leave:
mov eax,0xfee000b0		;eoi
mov dword [eax],0
pop rbx
pop rax
iretq

endofkeyboard:
;________________________________________


jmp endofinterrupt

;___________________________________
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
;____________________________________




endofinterrupt:
sti
