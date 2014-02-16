;init/[1,7]
[BITS 64]
    mov r8b,[0xa019]        ;r8=bit/点
    mov cl,3
    shr r8b,cl               ;r8=byte/点

    mov r9,r8
    mov cl,10
    shl r9d,cl              ;r9=1024*r8

    mov r10,r8
    mov cl,4
    shl r10d,cl              ;r10=16*r8

    mov r11,r8
    mov cl,14
    shl r11d,cl              ;r10=16*r8

    mov edi,[0xa028]        ;vesabase
    mov eax,0xffff00
    mov ecx,1024*768
yellow:
    mov [edi],eax
    add edi,r8d
    loop yellow

    mov edi,[0xa028]   ;开头

    xor rcx,rcx
    mov cx,600
fuckyou:
    mov esi,mai
    call utf8
    call utf8
    call utf8
    call utf8
    loop fuckyou
    
DIE:hlt
jmp DIE


;进:esi,edi
;出:esi,edi
;用:
;变:
utf8:
    push rcx
    xor rcx,rcx
    mov cl,16
  yihang:
    xor rax,rax
    lodsw
    push rcx
        mov cl,16
      yidian:
        mov bx,ax
        and bx,0x1
        cmp bx,1
        jne nimei
        mov dword [edi],0x00000000
      nimei:
        add edi,r8d
        shr ax,1
        loop yidian
    sub edi,r10d           ;每个字的行头
    add edi,r9d            ;下1行
    pop rcx
    loop yihang

    add edi,r10d            ;下个字的行头
    sub edi,r11d            ;上16行;现在edi=下个字开头

    mov eax,edi
    mov ebx,[0xa028]        ;ebx=vesabase
    sub eax,ebx             ;eax=相对距离

    cmp r8,4
    je modfour                  ;r8==4
    and eax,0x000003ff          ;r8==3则mod 1024
    cmp eax,0
    jne nochange
    jmp change

modfour:
    and eax,0x00000fff           ;mod4096
    cmp eax,0
    jne nochange

change:
    add edi,r11d
    sub edi,r9d
nochange:
    pop rcx
    ret              ;edi指向下一个字


mai:
db 0x00,0x00,0xFE,0x7F,0x00,0x60,0x20,0x33,0x60,0x03,0x48,0x03,0x18,0x03,0x30,0x01,0x20,0x21,0xFE,0x7F,0x80,0x01,0x80,0x03,0xC0,0x0E,0x70,0x18,0x1C,0x30,0x06,0x20
le:
db 0x00,0x00,0x00,0x10,0xFE,0x3F,0x00,0x1C,0x00,0x06,0x80,0x01,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0xF0,0x00,0xC0,0x00
ge:
db 0x80,0x00,0xC0,0x01,0xC0,0x01,0x60,0x03,0x30,0x06,0x98,0x0C,0x8C,0x39,0x82,0x70,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x80,0x00,0x00,0x00
biao:
db 0x00,0x00,0x00,0x01,0x00,0x21,0xFC,0x7F,0x00,0x11,0xF8,0x3F,0x00,0x41,0xFE,0xFF,0xC0,0x03,0x60,0x32,0x30,0x1E,0x3C,0x04,0x26,0x1F,0xE0,0x79,0x70,0xE0,0x20,0x00
