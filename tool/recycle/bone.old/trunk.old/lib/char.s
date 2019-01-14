bits 64




;________________________________
scan2anscii:

    lea esi,[rel translatetable]          ;table
    xor ecx,ecx
.search:
    cmp [esi+ecx],al            ;先把al里的扫描码转换成anscii给al
    je .return
    add ecx,2
    cmp ecx,(endoftranslatetable-translatetable)
    jb .search

.returnnothing:
    mov al,0x20
    ret

.return:
    ;inc esi
    ;lodsb
    mov al,[esi+ecx+1]
    ret
;____________________________________




translatetable:
db 0x0b, '0'
db 0x02, '1'
db 0x03, '2'
db 0x04, '3'
db 0x05, '4'
db 0x06, '5'
db 0x07, '6'
db 0x08, '7'
db 0x09, '8'
db 0x0a, '9'
db 0x1e, 'a'
db 0x30, 'b'
db 0x2e, 'c'
db 0x20, 'd'
db 0x12, 'e'
db 0x21, 'f'
db 0x22, 'g'
db 0x23, 'h'
db 0x17, 'i'
db 0x24, 'j'
db 0x25, 'k'
db 0x26, 'l'
db 0x32, 'm'
db 0x31, 'n'
db 0x18, 'o'
db 0x19, 'p'
db 0x10, 'q'
db 0x13, 'r'
db 0x1f, 's'
db 0x14, 't'
db 0x16, 'u'
db 0x2f, 'v'
db 0x11, 'w'
db 0x2d, 'x'
db 0x15, 'y'
db 0x2c, 'z'
db 0x4e, '+'
db 0x4a, '-'
db 0x37, '*'
db 0x35, '/'
db 0x1a, '['
db 0x1b, ']'
db 0x33, ','
db 0x34, '.'
db 0x39, ' '
db 0x0d, '='
db 0x7e, '~'
db 0xff,0xff
endoftranslatetable:




;________________________________
scan2hex:

    lea esi,[rel scan2hextable]          ;table
    xor ecx,ecx
.search:
    cmp [esi+ecx],al            ;先把al里的扫描码转换成anscii给al
    je .return
    add ecx,2
    cmp ecx,(endofscan2hextable-scan2hextable)
    jb .search

.returnnothing:
    mov al,0xff
    ret

.return:
    ;inc esi
    ;lodsb
    mov al,[esi+ecx+1]
    ret
;____________________________________




scan2hextable:
db 0x0b,0x0
db 0x02,0x1
db 0x03,0x2
db 0x04,0x3
db 0x05,0x4
db 0x06,0x5
db 0x07,0x6
db 0x08,0x7
db 0x09,0x8
db 0x0a,0x9
db 0x1e,0xa
db 0x30,0xb
db 0x2e,0xc
db 0x20,0xd
db 0x12,0xe
db 0x21,0xf
endofscan2hextable:




;_________________________________
character:
    push rcx

    movzx eax,al
    shl eax,9
    lea esi,[eax+0x110000]

    xor ecx,ecx
.lines:		;每排8个DWORD=4个QWORD
    lodsq
    mov [edi+ecx],rax
    lodsq
    mov [edi+ecx+8],rax
    lodsq
    mov [edi+ecx+16],rax
    lodsq
    mov [edi+ecx+24],rax

    add ecx,1024*4
    cmp ecx,1024*4*16
    jna .lines

.return:
    add edi,32
    pop rcx
    ret
;____________________________________




;______________________________
;character:
;    push rbx
;    push rcx
;
;    movzx eax,al
;    shl ax,4
;    lea esi,[eax+0x4000]
;
;    mov ecx,16           ;16行
;.yihang:
;    xor rax,rax
;    lodsb
;    push rcx
;        mov cl,8
;      .yidian:
;        shl al,1
;        jnc .bg
;        mov edx,[rel frontcolor]
;        mov [edi],edx
;        jmp .pointdone
;      .bg:
;        mov edx,[rel backcolor]
;        cmp edx,0xffffffff
;        je .pointdone
;        mov dword [edi],edx
;      .pointdone:
;        add edi,4
;        loop .yidian
;    sub edi,32           ;每个字的行头
;    add edi,1024*4            ;下1行
;    pop rcx
;    loop .yihang
;
;    add edi,32            ;下个字的行头
;    sub edi,4*1024*16            ;上16行;现在edi=下个字开头
;
;    mov eax,edi
;    mov ebx,0x1000000        ;ebx=vesabase
;    ;mov ebx,[0x3028]        ;ebx=vesabase
;    sub eax,ebx             ;eax=相对距离
;
;.modfour:
;    and eax,0x00000fff           ;mod4096
;    cmp eax,0
;    jne .nochange
;
;.change:
;    add edi,4*1024*16
;    sub edi,1024*4
;
;.nochange:
;    pop rcx
;    pop rbx
;
;    ret              ;edi指向下一个字
;______________________________________
;backcolor:dd 0
;frontcolor:dd 0xffffffff




;__________________________________
char:
	cmp al,0x80
	jae .blank
	cmp al,0x20
	jb .blank
	call character
	ret

	.blank:
	mov al,0x20
	call character
	ret
;____________________________________




;把一串字符直接显示到edi指定的屏幕位置
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




;把rbx的值直接显示到edi指定的屏幕位置
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
        call character
        pop rcx
        pop rbx
	loop .getaddress

	ret

	.notprint:
	add edi,32
	loop .getaddress

	ret
;________________________________
firstnonzero: db 0




;rbx的值转成anscii串放在下面string:
;___________________________________
itoa:
rbx2string:
	mov qword [rel string],0
	mov qword [rel string+8],0
	lea edi,[rel string+0xf]
	mov ecx,8		;只管低32位
.continue:
	mov al,bl
	and al,0xf
	add al,0x30
	cmp al,0x3a
	jb .skip
	add al,0x7
.skip:
	shr rbx,4
	mov [edi],al
	dec edi
	loop .continue

.return:
	ret
;___________________________________
string:db "0123456789abcdef"




;esi指向的anscii串转成数字放在value:里
;________________________________
string2data:
mov qword [rel value],0
mov ecx,16
.part:

lodsb                ;get one char

cmp al,0x30
jb .finish
cmp al,0x39          ;>0x39,maybe a~f
ja .atof
sub al,0x30          ;now its certainly 0~9
jmp .generate

.atof:
cmp al,0x61          ;[0x40,0x60],error
jb .finish
cmp al,0x66          ;>0x66,error
ja .finish
sub al,0x57          ;now its certainly a~f

.generate:
shl qword [rel value],4
add byte [rel value],al
loop .part

.finish:
ret
;_______________________________-
value:dq 0
