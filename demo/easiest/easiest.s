org 0x10000
[bits 64]

mov edi,[0x3028]	;get screen address
mov eax,0xaaaaaaaa	;color
mov ecx,1024*200	;how many times
rep stosd		;write screen

thenyousleep:hlt
jmp thenyousleep
