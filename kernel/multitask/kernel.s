%include "apic.s"
[BITS 64]
;_______________________________
cli
mov esi,0x30000
mov edi,0x100000
mov ecx,0x1000
rep movsq

mov esi,0x20000
mov edi,0x200000
mov ecx,0x1000
rep movsq

xor eax,eax
mov edi,0x80000
mov ecx,0x2000
rep stosq
;______________________________




;_________________________________________
;process 1
mov edi,0x80000
mov dword [edi],0x100120	;rip
mov byte [edi+8],0x08		;cs
pushf
pop rax
or rax,0x200
mov qword [edi+0x10],rax	;rflag
mov dword [edi+0x18],0x200000	;rsp
mov byte [edi+0x20],0x10	;ss
;__________________________________




;_____________________________________
;process 2
mov edi,0x80100
mov dword [edi],0x2002d0
mov byte [edi+8],0x08
pushf
pop rax
or rax,0x200
mov qword [edi+0x10],rax
mov dword [edi+0x18],0x300000
mov byte [edi+0x20],0x10
;________________________________




sti
mov rdx,0x100120
jmp rdx
