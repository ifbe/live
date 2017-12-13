%define vesahome 0x2000

[BITS 16]
startofscreen:

;__________________________________
	;buf
	mov eax,0xb8000
	mov [vesahome+0x0],eax
	;fmt
	mov [vesahome+0x8],eax
	;x
	mov word [vesahome+0x10],80
	;y
	mov word [vesahome+0x18],25

	jmp endofscreen
;___________________________________


paddingofscreen:
times 0x400-(paddingofscreen-startofscreen) db 0

endofscreen:
