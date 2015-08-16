%define idthome 0x3000
[bits 64]




;in:r8=number,r9=israddr
;___________________________________
isrinstall:
	mov rdi,r8
	shl rdi,4						;each=0x10(shift left 4=mul 16)
	add rdi,idthome					;add baseaddr
	mov rax,r9

	stosw							;idt.[1,0]=addr.[1,0]
	mov dword [edi],0x8e000008		;idt.[5,2]=0x8e000008
	add edi,4
	shr rax,16
	stosw							;idt.[7,6]=addr.[3,2]
	shr rax,16						;idt.[b,8]=addr.[7,4]
	stosd
	xor eax,eax						;idt.[f,c]=0
	stosd

	ret
;____________________________________