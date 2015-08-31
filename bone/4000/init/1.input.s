%define kbdhome 0x20000
%define bufsize 0x1000
[bits 64]




;_________________1111111111___________________
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
;_____________________________________________




;_____________222222222222222__________________
;isr21
	cli
	mov r8,0x21					;int 0x21	(0x20+1)
	lea r9,[rel keyboardisr]
	call isrinstall

;enable
	mov edi,0xfec00000
	mov dword [edi],0x10+2*0x1
	mov dword [edi+0x10],0x21
	mov dword [edi],0x10+2*0x1+1
	mov dword [edi+0x10],0

	jmp endof8042
;________________________________________




;______________________________________
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
;________________________________________




endof8042:
sti
