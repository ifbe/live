%include "apic.s"
%include "switch.s"
[BITS 64]


kernel:
cli


;_________________________________________
;process 1
lea edi,[rel pcb]
mov dword [edi],0x110000	;rip
mov byte [edi+8],0x08		;cs
pushf
pop rax
or rax,0x200
mov qword [edi+0x10],rax	;rflag
mov dword [edi+0x18],0x120000	;rsp
mov byte [edi+0x20],0x10	;ss
;__________________________________




;_____________________________________
;process 2
lea edi,[rel pcb+0x100]
mov dword [edi],0x120000
mov byte [edi+8],0x08
pushf
pop rax
or rax,0x200
mov qword [edi+0x10],rax
mov dword [edi+0x18],0x130000
mov byte [edi+0x20],0x10
;________________________________




lea rdx,[rel pcb]
mov [rel running],rdx

sti
lea rdx,[rel firsttask]
jmp rdx




pcb:
times 0x200 db 0
times 0x10000-($-$$)db 0




firsttask:
incbin "temp1"
times 0x20000-($-$$)db 0

secondtask:
incbin "temp2"
times 0x30000-($-$$)db 0
