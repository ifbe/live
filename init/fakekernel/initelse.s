bits 64
    mov qword [rel addr],0x6000          ;r14 memory pointer
    mov qword [rel offset],0             ;r15 offset pointer

    mov rdi,0x800
    mov [0xff8],rdi

    xor rax,rax
    mov [0xff0],rax         ;keyboard buffer end

    mov ecx,0xfe
    rep stosq

    jmp ramdump
