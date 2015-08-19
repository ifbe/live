;屏幕全白？
[BITS 64]
    mov bl,[0x3019]        ;bitperpixel
    mov cl,3
    shr bl,cl
    mov edi,[0x3028]        ;vesabase
    mov eax,0xffffff

    mov ecx,0x40000
line:
    mov [edi],eax
    add edi,ebx
    loop line
    
DIE:hlt
jmp DIE
