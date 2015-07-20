%define journalhome 0xd00000
%define journalsize 0x100000
;清空/journal
    mov edi,journalhome
    mov ecx,journalsize
    xor rax,rax
    rep stosb
    mov rax,"current"
    mov [journalhome+journalsize-0x10],rax
