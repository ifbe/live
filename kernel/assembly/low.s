;[1,7]
%define address 0x100000
[BITS 64]      
kernel:
;    mov edi,0xa73a0
    mov edi,0xa0000
;    mov ecx,0x1f40               ;full screen
;    mov rax,0x000000050000000d
;    rep stosq

    xor ah,ah
keyboard:
    in al,0x64
    and al,0x02
    cmp al,0
    jne keyboard

    in al,0x60
    ;mov edi,0xb8f9c
    ;mov dl,0x01
    ;call display_al

    cmp ah,al
    je keyboard

    cmp al,0x48            ;keyboard process
    je up
    cmp al,0x50
    je down
    cmp al,0x4b
    je left
    cmp al,0x4d
    je right
    cmp al,0x39
    je bomb
    cmp al,0x01
    je restart
    mov ah,al
    jmp keyboard

up:
    sub edi,0x140
    jmp display
down:
    add edi,0x140
    jmp display
left:
    sub edi,0x1
    jmp display
right:
    add edi,0x1
    jmp display
bomb:
    mov edi,0xa0000
    jmp display
restart:
    mov al,0xfe
    out 0x64,al
display:
    push rdi

    mov cl,8                   ;i row
    .height:
        push cx
        mov cl,8
        .length:
            mov [edi],al
            inc edi
            loop .length
        add edi,0x138
        pop cx
        loop .height

    pop rdi
    mov ah,al
    jmp keyboard

DIE:hlt
    jmp DIE
