;清空/journal
    mov ecx,0x8000
    mov edi,0x40000
    xor rax,rax
    rep stosq
    mov rax,"current"
    mov [0x7fff0],rax
