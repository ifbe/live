%define vesahome 0x2000

[BITS 16]
startofscreen:

;__________________________________________
	mov ax,0x13
	int 0x10

	cld
	mov rdi,0x2000
	mov rax,0xb8000		;buf
	stosq
	mov rax,666666		;fmt
	stosq
	mov rax,320		;x
	stosq
	mov rax,240		;y
	stosq

	jmp endofscreen
;___________________________________________

paddingofscreen:
times 0x400-(paddingofscreen-startofscreen) db 0

endofscreen: