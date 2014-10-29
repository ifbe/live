;init/[1,7]
[BITS 64]
changecr3:
    ;清空
    xor rax,rax
    mov edi,0x10000
    mov ecx,0x10000
    rep stosq

    ;___________&pdpt放入pml4_
    mov edi,0x10000
    mov eax,0x11000       ;pdpt地址
    or eax, 0x3          ;存在&&可写
    mov [edi],eax     ;放入pml4

;_______________&pd放入pdpt
    mov edi,0x11000
    mov eax,0x12003
    mov [edi],eax

;____________真实地址放入pd
    mov edi,0x12000
    mov eax, 0x83
    mov [edi],eax
    add edi,8
    mov eax,0xfc000083
    mov [edi],eax

    mov rax,0x10000
    mov cr3,rax

    mov rax,0xffff
    mov edi,0x200000
    mov ecx,0x40000           ;max
    rep stosq

DIE:hlt
jmp DIE
