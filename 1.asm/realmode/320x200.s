%define vesahome 0x2000

[BITS 16]
startofscreen:

;__________________________________________
	mov ax,0x13
	int 0x10

	;buf
	mov eax,0xa0000
	mov [vesahome],eax
	;fmt
	mov [vesahome+8],eax
	;x
	mov eax,320
	mov [vesahome+0x10],eax
	;y
	mov eax,200
	mov [vesahome+0x18],eax

	jmp endofscreen
;___________________________________________

paddingofscreen:
times 0x400-(paddingofscreen-startofscreen) db 0

endofscreen:
