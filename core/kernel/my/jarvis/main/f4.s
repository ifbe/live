bits 64
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>----console----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;[0x120000，0x12ffff]属于console，弄得好看一点



;_______________________________________________
function4:

cmp al,0
je console		;do nothing
cmp al,0x3b
je f1
cmp al,0x3c
je f2
cmp al,0x3d
je f3
cmp al,0x3e
je f4
cmp al,0x3f
je f5
cmp al,0x1c
je f4enter
cmp al,0x0e
je f4backspace

cmp al,0x80
ja console
jmp f4other
;_______________________________________________________________________________


;_______________________________
f4backspace:

cmp byte [rel length],8
jna console

;lea ebx,[rel line0]
mov ebx,[0x12fff8]
add ebx,0x120000
dec byte [rel length]
add ebx,[rel length]
mov byte [ebx],0

jmp console
;_________________________________________


;_____________________________________
f4other:

cmp byte [rel length],128
jae console

call scan2anscii

record:
mov ebx,[0x12fff8]
add ebx,0x120000
add ebx,[rel length]
mov [ebx],al
inc byte [rel length]
jmp console
;___________________________________


;___________________________________
f4enter:

call explainarg

jmp searchinhere
.notinhere:

jmp searchinmemory
.notinmemory:

notfound:
	call checkandchangeline
	mov edi,[0x12fff8]
	add edi,0x120000
	mov dword [edi],"notf"
	mov dword [edi+4],"ound"

scroll:
	call checkandchangeline
	mov edi,[0x12fff8]
	add edi,0x120000
	mov dword [edi],"[   "
	mov dword [edi+4],"/]$ "
	mov dword [rel length],8
	jmp console
;_________________________________




;举例：
;进来时esi指向"   love     123456 "
;出去时arg0="love"，[arg1]=123456，esi指向最后的空格
;本行全空会执行上一次的指令
;因为上一次取得的arg0和arg1没有被改变
;______________________________________
explainarg:
	mov esi,[0x12fff8]
	add esi,0x120008			;[   /]


	;;;;;;;;;;;;吃掉esi指向的最开始的空格
	mov ecx,8
.eatspace0:
	cmp byte [esi],0x20		;先检查
	ja .ate0			;已经吃光了，下一步
	inc esi				;吃一个空格
	loop .eatspace0
	jmp .return			;全是空格->出错了直接返回
.ate0:


	;;;;;;;;;;;;;现在esi指向l
.fetcharg0:
        mov qword [rel arg0],0		;先清理
        mov qword [rel arg0+8],0	;先清理
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
        mov qword [rel arg1],0		;先清理
        mov qword [rel arg1+8],0	;先清理
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


.return:
	ret
;____________________________________________
arg0:times 16 db 0		;本函数运行完的输出结果
arg1:times 16 db 0




;___________________________________
searchinhere:

lea esi,[rel arg0]
cmp dword [esi],"powe"		;if [esi]=="powe"
jne skippoweroff
cmp dword [esi+4],"roff"	;if [esi+4]=="roff"
jne skippoweroff
cmp byte [esi+8],0		;if [esi+8]=0x20
je poweroff
skippoweroff:

lea esi,[rel arg0]
cmp dword [esi],"rebo"
jne skipreboot
cmp word [esi+4],"ot"
jne skipreboot
cmp byte [esi+6],0
je reboot
skipreboot:

lea esi,[rel arg0]
cmp dword [esi],"exit"
jne skipexit
cmp byte [esi+4],0
je poweroff
skipexit:

lea esi,[rel arg0]
cmp dword [esi],"clea"
jne skipclear
cmp byte [esi+4],'r'
je clear
skipclear:

lea esi,[rel arg0]
cmp word [esi],"ls"
je ls

lea esi,[rel arg0]
cmp dword [esi],"test"
je test

lea esi,[rel arg0]
cmp dword [esi],"call"
je cast

lea esi,[rel arg0]
cmp dword [esi],"jump"
je jump

lea esi,[rel arg0]
cmp dword [esi],"int"
je enterinterrupt

jmp f4enter.notinhere
;_____________________________________




;找到就执行
;____________________________________
searchinmemory:
	mov rax,[rel arg0]
	cmp rax,0x20
	jb .return

	mov esi,0x180000		;/bin
.search:
	cmp [esi],rax
	je .find
	add esi,0x10
	cmp esi,0x200000
	jb .search
	jmp f4enter.notinmemory

.find:
	mov rbx,[esi+8]
	mov [rel explainedarg0],rbx	;取得函数地址，保存在某处

	call checkandchangeline

	mov rbx,[rel explainedarg0]
	call rbx2string
	lea esi,[rel string]
	mov edi,[0x12fff8]
	add edi,0x120000
	movsq
	movsq

	mov edi,[0x12fff8]
	add edi,0x120000
	add edi,16+8
	lea rsi,[rel arg1]
	movsq
	movsq

	lea rdi,[rel arg1]
	call [rel explainedarg0]

	jmp scroll

.return:
	jmp f4enter.notinmemory
;__________________________________________
explainedarg0:dq 0	;解释完是函数地址




;__________________________________
test:
.cleanmemory:
	mov edi,0x2000000
	mov ecx,0x2000
	xor rax,rax
	rep stosq
.search:
	mov esi,0x180000
	.continue:
	cmp dword [esi],"load"
	je .load
	add esi,0x10
	cmp esi,0x200000
	jb .continue
	jmp notfound
.load:
	mov rdx,[esi+8]
	lea rdi,[rel arg1]
	call rdx
.check:
	cmp dword [0x2000000],0
	je notfound
.execute:
	mov rax,0x2000000
	call rax
.return:
	jmp scroll
;__________________________________




;____________________________________
clear:
mov edi,0x120000
mov ecx,128*0x30
xor rax,rax
rep stosb

mov edi,0x120000
mov rax,"[   /]$ "
stosq

mov dword [0x12fff8],0
mov dword [rel length],8
jmp console
;_______________________________________




;_________________________
ls:
	call checkandchangeline		;get new edi
	mov edi,[0x12fff8]
	add edi,0x120000
	mov esi,0x4c0000
	xor ecx,ecx
.continue:
	cmp dword [esi],0
	je scroll
	movsq
	add esi,0x18
	add edi,0x8
	inc ecx

	cmp ecx,0x200
	jae scroll
	test ecx,0x7
	jnz .continue
	call checkandchangeline
	mov edi,[0x12fff8]
	add edi,0x120000
	jmp .continue
;_________________________




;_______________________________
cast:

lea esi,[rel arg1]
call string2data
call [rel value]
jmp scroll
;_______________________________




;_______________________________
jump:

lea esi,[rel arg1]
call string2data
jmp [rel value]
;_______________________________




;_______________________________
enterinterrupt:

int3
jmp scroll
;_______________________________




;_________________________________
checkandchangeline:
	cmp dword [0x12fff8],0x80*47
	jae .move

	add dword [0x12fff8],128		;no:line+1
	jmp .return

	.move:					;yes:move
	push rsi
	push rcx
	mov esi,0x120080
	mov edi,0x120000
	mov ecx,128*0x30
	cld
	rep movsb
	pop rcx
	pop rsi

	.return:
	ret					;now line=a blank line
;____________________________________




;___________________________________________
console:
mov edi,0x1c00000
mov ecx,1024*750
xor eax,eax
rep stosd


mov dword [rel looptimes],0
.lines:
    mov edi,0x1c00000
    mov eax,4*1024*16
    imul eax,[rel looptimes]
    add edi,eax

    mov esi,0x120000
    mov eax,[rel looptimes]
    shl eax,7
    add esi,eax

    mov cl,128
    .continue:
    mov al,[esi]
    push rsi
    call char
    pop rsi
    inc esi
    dec cl
    jnz .continue

    inc byte [rel looptimes]
    cmp byte [rel looptimes],0x30
    jb .lines


    mov esi,0x1c00000
    mov edi,[0x3028]
    mov bl,[0x3019]
    shr bl,3
    movzx ebx,bl
    mov ecx,1024*768
.continuescreen:
    lodsd
    mov [edi],eax
    add edi,ebx
    loop .continuescreen

jmp forever
;________________________________________
looptimes:dd 0
length:dq 8
