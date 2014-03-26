bits 64
;_________________________
f1:
    mov dword [rel decide+1],function1-(decide+5)		;selfmodify
    jmp ramdump
f2:
    mov dword [rel decide+1],function2-(decide+5)		;selfmodify
    jmp picture
f3:
    mov dword [rel decide+1],function3-(decide+5)		;selfmodify
    jmp cyberspace
f4:
    mov dword [rel decide+1],function4-(decide+5)		;selfmodify
    jmp console


esc:
turnoff:
mov dx,[0x4fc]
mov ax,[0x4fe]
or ax,0x2000
out dx,ax

;__________________________




;进:esi,edi
;出:esi,edi
;用:
;变:
;______________________________
char:
    push rbx
    push rcx

    movzx eax,al
    shl ax,4
    lea esi,[eax+0x7000]

    mov ecx,16           ;16行
.yihang:
    xor rax,rax
    lodsb
    push rcx
        mov cl,8
      .yidian:
        shl al,1
        jnc .bg
        mov edx,[rel frontcolor]
        mov [edi],edx
        jmp .pointdone
      .bg:
        mov edx,[rel backcolor]
        cmp edx,0xffffffff
        je .pointdone
        mov dword [edi],edx
      .pointdone:
        add edi,4
        loop .yidian
    sub edi,32           ;每个字的行头
    add edi,1024*4            ;下1行
    pop rcx
    loop .yihang

    add edi,32            ;下个字的行头
    sub edi,4*1024*16            ;上16行;现在edi=下个字开头

    mov eax,edi
    mov ebx,0x100000        ;ebx=vesabase
    ;mov ebx,[0x3028]        ;ebx=vesabase
    sub eax,ebx             ;eax=相对距离

.modfour:
    and eax,0x00000fff           ;mod4096
    cmp eax,0
    jne .nochange

.change:
    add edi,4*1024*16
    sub edi,1024*4

.nochange:
    pop rcx
    pop rbx

    ret              ;edi指向下一个字
;______________________________________
backcolor:dd 0
frontcolor:dd 0xffffffff




;____________________________________
writescreen:
    mov esi,0x100000
    mov edi,[0x3028]
    mov bl,[0x3019]
    shr bl,3
    movzx ebx,bl
    mov ecx,1024*768

.continue:
    lodsd
    mov [edi],eax
    add edi,ebx
    loop .continue

ret
;_________________________________
