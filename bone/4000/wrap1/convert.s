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








;传参：r8,r9,r10,r11......
;___________________________________
data2string:
itoa:
	mov rbx,r8
	mov qword [rel string],0
	mov qword [rel string+8],0

	lea edi,[rel string+0xf]
	mov ecx,16		;只管低32位
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
string:db "0123456789abcdef",0xa,0








;传参：r8,r9,r10,r11,r12......
;esi指向的anscii串转成数字放在value:里
;________________________________
string2data:
atoi:
	mov rsi,r8

	mov qword [rel value],0
	mov ecx,16
.part:
	lodsb                ;get one char
	cmp al,0x30
	jb .return
	cmp al,0x39          ;>0x39,maybe a~f
	ja .atof
	sub al,0x30          ;now its certainly 0~9
	jmp .generate

.atof:
	cmp al,0x61          ;[0x40,0x60],error
	jb .return
	cmp al,0x66          ;>0x66,error
	ja .return
	sub al,0x57          ;now its certainly a~f

.generate:
	shl qword [rel value],4
	add byte [rel value],al
	loop .part

.return:
	ret
;_______________________________-
value:dq 0








;传参：r8,r9,r10,r11......
;分析字符串，把结果放到下面等人来取
;__________________________________________________
buf2arg:
	mov rsi,r8					;buffer地址
	mov qword [rel arg0],0		;先清理
	mov qword [rel arg0+8],0
	mov qword [rel arg1],0
	mov qword [rel arg1+8],0
	mov qword [rel arg2],0
	mov qword [rel arg2+8],0


;;;;;;;;;;;;吃掉esi指向的最开始的空格
	mov ecx,8
.eatspace0:
	cmp byte [esi],0x20		;先检查
	ja .ate0			;已经吃光了，下一步
	inc esi				;吃一个空格
	loop .eatspace0
	jmp .return			;全是空格->出错了直接返回
.ate0:


;;;;;;;;;;;;;现在esi指向0
.fetcharg0:
        lea rdi,[rel arg0]		;edi=目标地址
        mov ecx,16
.continue0:
        lodsb				;取一个
        cmp al,0x20
        jbe .finisharg0			;小于0x20或者
        cmp al,0x7a
        ja .finisharg0			;大于0x80都是错，直接返回
        stosb				;正常的话往目的地放
        loop .continue0
.finisharg0:


;;;;;;;;;;;;吃掉esi指向的中间的空格
	mov ecx,8
.eatspace1:
	cmp byte [esi],0x20		;先检查
	ja .ate1			;已经吃光了，下一步
	inc esi				;吃一个空格
	loop .eatspace1
	jmp .return			;全是空格->出错了直接返回
.ate1:



;;;;;;;;;;;现在esi指向1
.fetcharg1:
        lea rdi,[rel arg1]		;edi=目标地址
        mov ecx,16
.continue1:
        lodsb				;取一个
        cmp al,0x20
        jbe .finisharg1			;小于0x20或者
        cmp al,0x7a
        ja .finisharg1			;大于0x80都是错，直接返回
        stosb				;正常的话往目的地放
        loop .continue1
.finisharg1:



;;;;;;;;;;;;吃掉esi指向的中间的空格
	mov ecx,8
.eatspace2:
	cmp byte [esi],0x20		;先检查
	ja .ate2			;已经吃光了，下一步
	inc esi				;吃一个空格
	loop .eatspace2
	jmp .return			;全是空格->出错了直接返回
.ate2:


;;;;;;;;;;;现在esi指向2
.fetcharg2:
        lea rdi,[rel arg2]		;edi=目标地址
        mov ecx,16
.continue2:
        lodsb				;取一个
        cmp al,0x20
        jbe .finisharg2			;小于0x20或者
        cmp al,0x7a
        ja .finisharg2			;大于0x80都是错，直接返回
        stosb				;正常的话往目的地放
        loop .continue2
.finisharg2:


.return:
	call checkandchangeline
	ret
;____________________________________________
arg0msg:db "    arg0="
arg0:times 16 db 0		;本函数运行完的输出结果

arg1msg:db "    arg1="
arg1:times 16 db 0

arg2msg:db "    arg2="
arg2:times 16 db 0