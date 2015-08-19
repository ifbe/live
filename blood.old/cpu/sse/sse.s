;init/[1,6]
[BITS 64]
    mov edi,0xB8000
    mov ecx,0x07d0
    mov ax,0x7720
    rep stosw

    mov rdi,0x10000
    movsd xmm0,[edi]

DIE:hlt
jmp DIE
