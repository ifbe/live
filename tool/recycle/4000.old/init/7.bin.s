%define binhome 0x70000
[bits 64]

;清空/bin
    mov ecx,0x10000
    mov edi,binhome
    xor rax,rax
    rep stosb
