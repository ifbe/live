%define binhome 0x180000

;清空/bin
    mov ecx,0x10000
    mov edi,binhome
    xor rax,rax
    rep stosq
