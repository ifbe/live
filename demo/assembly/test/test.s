;init/[1,6]
[bits 64]
mov edi,0xb8000
mov rax,0xf067f06ef06ff06c
stosq
mov rax,0xf065f064f06ff06d
stosq
death:hlt
jmp death

