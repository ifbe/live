[bits 64]
mov rsp,0x1000000    ;16m
mov esi,0x20000
mov edi,0x100000
mov ecx,0x2000
rep movsq

mov rdx,0x100c30
call rdx

death:
    hlt
    jmp death
