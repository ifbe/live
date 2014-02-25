[bits 64]
mov rax,0x77777777
mov ecx,0x10000
mov edi,[0x3028]
rep stosq

death:hlt
jmp death
