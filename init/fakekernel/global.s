bits 64
;_________________________
f1:
    mov dword [rel decide+1],function1-(decide+5)	;selfmodify
    jmp ramdump
f2:
    mov dword [rel decide+1],function2-(decide+5)	;selfmodify
    jmp picture
f3:
    mov dword [rel decide+1],function3-(decide+5)	;selfmodify
    jmp cyberspace
f4:
    mov dword [rel decide+1],function4-(decide+5)	;selfmodify
    jmp console
;___________________________________________


;__________________________
turnoff:
mov dx,[0x4fc]
mov ax,[0x4fe]
or ax,0x2000
out dx,ax
;__________________________




;________________________________
translate:
    mov esi,0x7800                 ;table
.search:
    cmp [esi],al            ;先把al里的扫描码转换成anscii给al
    je .convert
    add esi,2
    cmp esi,0x7880
    jb .search
    mov esi,0x7200           ;no,blank
    jmp .finish
.convert:
    inc esi
    lodsb
.finish:
    ret
;____________________________________




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
    mov ebx,0x1000000        ;ebx=vesabase
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




;_______________________________
message:
mov ecx,16
.continue:
push rcx
push rsi
lodsb
call char
pop rsi
pop rcx
inc esi
loop .continue
ret
;______________________________




;_________________________________
dumprbx:
mov ecx,16
.getaddress:
        rol rbx,4
        mov al,bl
        and al,0x0f

        push rbx
        push rcx
        call char
        pop rcx
        pop rbx

        loop .getaddress
ret
;________________________________




;__________________________________
address:
mov dword [rel frontcolor],0xffffffff

mov rbx,[rel addr]
mov edi,0x1000000+4*896
call dumprbx

mov rbx,[rel addr]
add rbx,[rel offset]
mov edi,0x1000000+4*1024*16+4*896
call dumprbx
;________________________________




;____________________________________
writescreen:
    mov esi,0x1000000
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




addr:dq 0
offset:dq 0
temp:dq 0

linenumber:dd 0x00
length:dd 0

line0:times 128 db ' '
line1:times 128 db ' '
lines:times 128*30 db ' '
linenull:times 128 db ' '

