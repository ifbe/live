bits 64
;_________________________
f1:
	lea rax,[rel function1]
	mov [rel screenwhat],rax
	jmp ramdump
f2:
	lea rax,[rel function2]
	mov [rel screenwhat],rax
	jmp picture
f3:
	lea rax,[rel function3]
	mov [rel screenwhat],rax
	jmp cyberspace
f4:
	lea rax,[rel function4]
	mov [rel screenwhat],rax
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
scan2anscii:
    mov esi,0x6800                 ;table
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




;___________________________________
scan2hex:
cmp al,0xb
je .return0
cmp al,0x1e
je .returna
cmp al,0x30
je .returnb
cmp al,0x2e
je .returnc
cmp al,0x20
je .returnd
cmp al,0x12
je .returne
cmp al,0x21
je .returnf
cmp al,0xb
ja .wrong

.return:
dec al
ret
.wrong:
mov al,0xff
ret
.return0:
xor al,al
ret
.returna:
mov al,0xa
ret
.returnb:
mov al,0xb
ret
.returnc:
mov al,0xc
ret
.returnd:
mov al,0xd
ret
.returne:
mov al,0xe
ret
.returnf:
mov al,0xf
ret

;__________________________________




;______________________________
char:
    push rbx
    push rcx

    movzx eax,al
    shl ax,4
    lea esi,[eax+0x6000]

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
cmp al,0x20
ja .thisdone
add edi,32
jmp .notprint
.thisdone:
call char
.notprint:
pop rsi
pop rcx
inc esi
loop .continue
ret
;______________________________




;_________________________________
dumprbx:
mov ecx,16
mov byte [rel firstnonzero],0
.getaddress:
        rol rbx,4
        mov al,bl
        and al,0x0f

	cmp byte [rel firstnonzero],0		;have first non zero?
	ja .print
	cmp ecx,1				;last value?
	je .print

	cmp al,0				;not print 0
	je .notprint

	.setsignal:				;now we have a non zero
	mov byte [rel firstnonzero],1

	.print:
        push rbx
        push rcx
        call char
        pop rcx
        pop rbx
	loop .getaddress

ret
	.notprint:
	add edi,32
	loop .getaddress
;________________________________
firstnonzero: db 0




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




screenwhat:dq 0
addr:dq 0
offset:dq 0
