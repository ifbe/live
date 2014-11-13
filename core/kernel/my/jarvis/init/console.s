bits 64
%define consolehome 0x120000


;___________________________________________
    mov qword [rel addr],0x40000         ;r14 memory pointer
    mov qword [rel offset],0x420         ;r15 offset pointer

;清空/console
    mov edi,consolehome
    mov rax,"[   /]$ "
    stosq
    mov ecx,128*0x30-8
    xor rax,rax
    rep stosb

    mov rax,"current"
    mov [consolehome+0xfff0],rax
    mov qword [consolehome+0xfff8],0
;____________________________________________________
