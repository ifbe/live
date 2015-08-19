%define whathome 0x60000

;清空/bin
    mov ecx,0x10000
    mov edi,whathome
    xor rax,rax
    rep stosb
