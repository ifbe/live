;init/[1,7]
[BITS 64]      
kernel:
    ;竖
    mov al,0x01
    mov edi,0xa2860
    mov cl,0x75
one:
    mov [edi],al
    add edi,0x140
    loop one

    ;横折
    mov al,0x02 
    mov edi,0xa2860
    mov cl,0x80
    rep stosb
    mov cl,0x75
two:
    mov [edi],al
    add edi,0x140
    loop two

    ;撇
    mov edi,0xa3150
    mov al,0x03
    mov cl,0x10
three:
    mov [edi],al
    add edi,0x13e
    loop three


    ;捺
    mov edi,0xa3170
    mov al,0x04
    mov cl,0x10
four:
    mov [edi],al
    add edi,0x142
    loop four

    ;竖
    mov al,0x05
    mov edi,0xa5940
    mov cl,0x40
five:
    mov [edi],al
    add edi,0x140
    loop five

    ;横折
    mov al,0x06 
    mov edi,0xa5940
    mov cl,0x40
    rep stosb
    mov cl,0x40
six:
    mov [edi],al
    add edi,0x140
    loop six

    ;横
    mov cl,0x40
    mov al,0x07 
    mov edi,0xaa800
    rep stosb

    ;横
    mov cl,0x80
    mov al,0x08 
    mov edi,0xab820
    rep stosb


DIE:hlt
    jmp DIE
