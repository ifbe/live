%define address 0x10000
;init/[1,6]
[BITS 64]      
;______________环境设置____________________
;进:
;出:
;用:
;变:rax,edi,ecx，*0xb8000~*0xbffff
;_____________clear screen___________________
clearscreen:
    xor rax,rax
    mov edi,0xB8000
    mov ecx,0x07d0
    rep stosw
;________________________________________


;进:
;出:
;用:
;变:
;无限循环________________________________________
forever:
    mov esi,0x9000
    mov rbp,0x9000
keyboard:
    in al,0x64
    test al,0x01
    jz keyboard

    in al,0x60
    mov edi,0xb8f9c
    mov dl,0x01
    call alhex

    cmp al,0x48            ;keyboard
    je up
    cmp al,0x50
    je down
    cmp al,0x4b
    je left
    cmp al,0x4d
    je right
    cmp al,0x39
    je space
    cmp al,0x1c
    je enter
    cmp al,0x0f
    je tab
    cmp al,0x29
    je command
    cmp al,0x01
    je turnoff
    jmp keyboard
;_______________________________________


;进:rbp,dh    
;出:rbp,dh
;变:rbp,dh
;用:view,anscii
;_________________________________
up:
    sub rbp,0x20
    cmp dh,0
    je view
    jmp anscii
down:
    add rbp,0x20              ;a row
    cmp dh,0
    je view
    jmp anscii
left:
    sub rbp,0x1000
    cmp dh,0
    je view
    jmp anscii
right:
    add rbp,0x1000             ;a page
    cmp dh,0
    je view
    jmp anscii
space:
    mov ebp,0x9000
    cmp dh,0
    je view
    jmp anscii
enter:
    mov ebp,[esi]             ;a page
    add esi,0x10
    cmp dword [esi],0
    jne huitou
    mov esi,0x9000
  huitou:
    cmp dh,0
    je view
    jmp anscii
restart:
    mov al,0xfe
    out 0x64,al
turnoff:
    mov dx,[0x4fc]
    mov ax,[0x4fe]
    or ax,0x2000
    out dx,ax
tab:
    cmp dh,1
    mov dh,0
    je view
    jmp anscii
command:
    call special
    je view
;___________________________________________


;进:
;出:
;用:
;变:rax,edi,ecx，*0xb8000~*0xbffff
;__________________________________________
anscii:
    push rbp
    push rax
    mov edi,0xb8000

    mov rbx,rbp
    mov dl,0x04
    mov cx,24
.address:
    call displayrbx
    add edi,0x80
    add rbx,0x20
    loop .address

    mov edi,0xB8020
    mov ecx,24
.clearchar:
    push rcx
    mov ecx,0x0040
    mov ax,0x0720
    rep stosw
    add edi,0x20
    pop rcx
    loop .clearchar

;print char
    mov edi,0xb8020
    mov dl,0x02

    mov cx,24
.print:
        push cx
        mov cx,0x20
    .row:
        mov al,[rbp]
        add edi,2
        call al_anscii
        inc rbp
        loop .row

    add edi,0x20
    pop cx
    loop .print


    pop rax
    pop rbp
    mov ah,al
    mov dh,1
    jmp keyboard
;________________________________________________________


;进:
;出:
;用:
;变:
;___________________________________________________
view:
    push rbp
    push rax
    mov rbx,rbp
    mov edi,0xb8000

    mov dl,0x04
    mov cx,24
.address:
    call displayrbx
    add edi,0x80
    add rbx,0x20
loop .address

    mov edi,0xb8020 
    mov dl,0x02
    mov cx,24
.printmem:

    push cx
    mov cx,0x8
.onerow:
    mov eax,[rbp]
    call eaxhex
    add rbp,4
    loop .onerow

    add edi,0x20
    pop cx
    loop .printmem
    sub rbp,0x280
    pop rax
    pop rbp
    mov ah,al
    jmp keyboard
;_______________________________________________


;进:rbx
;出:
;用:alhex
;变:rax,edi,ecx
;________________________________________________
displayrbx:
    push rax
    push rcx
    mov cx,8
.process_rbx:
    push rcx
    mov cl,8
    rol rbx,cl
    mov al,bl
    call alhex
    pop rcx
    loop .process_rbx
    pop rcx
    pop rax
    ret

eaxhex:
    call alhex
    shr eax,8
    call alhex
    shr eax,8
    call alhex
    shr eax,8
    call alhex
    ret

;进:
;出:
;用:process
;变:rax,rcx
alhex:
    push rax
    mov ah,al
    shr al,4
    call process
    mov al,ah
    and al,0x0f
    call process
    pop rax
    ret

;进:al
;出:
;用:al_anscii
;变:al
process:
    add al,0x30
    cmp al,0x3a
    jb al_anscii
    add al,0x07  

al_anscii:
    mov byte [edi],al
    inc edi
    mov byte [edi],dl
    inc edi
    ret
;__________________________________________________


special:
