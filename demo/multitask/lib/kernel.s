%include "lib/apic.s"
%include "lib/switch.s"
[BITS 64]


kernel:
cli


;_________________________________________
;process 1
lea rbx,[rel process1]

lea edi,[rel pcb]
mov [edi],rbx			;rip
mov byte [edi+8],0x08		;cs
pushf
pop rax
or rax,0x200
mov qword [edi+0x10],rax	;rflag
add rbx,0x10000
mov [edi+0x18],rbx		;rsp
mov byte [edi+0x20],0x10	;ss
;__________________________________




;_____________________________________
;process 2
lea rbx,[rel process2]

lea edi,[rel pcb+0x100]
mov [edi],rbx
mov byte [edi+8],0x08
pushf
pop rax
or rax,0x200
mov qword [edi+0x10],rax
add rbx,0x10000
mov [edi+0x18],rbx
mov byte [edi+0x20],0x10
;________________________________




lea rdx,[rel pcb]
mov [rel running],rdx

sti
lea rdx,[rel process1]
jmp rdx




pcb:
times 0x200 db 0
times 0x10000-($-$$)db 0




process1:
incbin "temp1"
times 0x20000-($-$$)db 0

process2:
incbin "temp2"
times 0x30000-($-$$)db 0

times 0x100000-($-$$)db 0
