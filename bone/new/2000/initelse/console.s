%define consolehome 0xc00000
%define consolesize 0x100000
[bits 64]




;___________________________________________
;清空/console
    mov edi,consolehome
    mov rax,"[   /]$ "
    stosq
    mov ecx,consolesize-8
    xor rax,rax
    rep stosb

    mov rax,"current"
    mov [consolehome+consolehome-0x10],rax
    mov qword [consolehome+consolehome-8],0
;____________________________________________________
