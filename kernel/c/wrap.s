[bits 64]
mov rsp,0x200000
mov esi,0x30000
mov edi,0x100000
mov ecx,0x2000
rep movsq

mov rdx,0x100120
call rdx

death:
    hlt
    jmp death
