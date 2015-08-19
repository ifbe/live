org 0x10000
[bits 64]
;__________________int3h______________________
breakpoint:
mov rax,breakpointisr
mov edi,0x1030
call idtinstaller

jmp breakpointdone

breakpointisr:
inc qword [0xff20]          ;test
iretq

breakpointdone:
;_____________________________________

death:hlt
jmp death
