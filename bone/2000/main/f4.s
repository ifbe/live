%define screeninfo 0x1000
%define binhome 0x6000
%define binsize 0x1000
%define consolehome 0xc00000
%define consolesize 0x100000
[bits 64]




;___________________________________________
;清空/console
f4init:
	mov edi,consolehome
	mov ecx,consolesize
	xor rax,rax
	rep stosb

	mov edi,consolehome
	lea esi,[rel welcomemessage]
	mov ecx,19
	rep movsb
	
	mov rax,"current"
	mov [consolehome+consolehome-0x10],rax
	mov qword [consolehome+consolesize-8],0x80

	ret
;____________________________________________________








;___________________________________________
f4show:
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

	mov esi,consolehome
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
	mov edi,[screeninfo+0x28]
	mov bl,[screeninfo+0x19]
	shr bl,3
	movzx ebx,bl
	mov ecx,1024*768
.continuescreen:
	lodsd
	mov [edi],eax
	add edi,ebx
	loop .continuescreen

	ret
;________________________________________
looptimes:dd 0
length:dq 8








;_______________________________________________
f4event:
	cmp al,0x80
	jae .return


.backspace:
	cmp al,0x0e
	jne .notbackspace
	cmp byte [rel length],8
	jna .return
	mov ebx,[consolehome+consolesize-8]
	add ebx,consolehome
	dec byte [rel length]
	add ebx,[rel length]
	mov byte [ebx],0
	ret
.notbackspace:


.enter:
	cmp al,0x1c
	jne .notenter
	call explainarg
	jmp searchhere
.notinhere:
	jmp searchmemory
.notinmemory:
	;jmp searchdisk
.notfound:
	call checkandchangeline
	mov edi,[consolehome+consolesize-8]
	add edi,consolehome
	mov dword [edi],"notf"
	mov dword [edi+4],"ound"
.scroll:
	call checkandchangeline
	mov edi,[consolehome+consolesize-8]
	add edi,consolehome
	mov dword [edi],"[   "
	mov dword [edi+4],"/]$ "
	mov dword [rel length],8
	ret
.notenter:


.other:
	cmp byte [rel length],128
	jae .return
	call scan2anscii
	mov ebx,[consolehome+consolesize-8]
	add ebx,consolehome
	add ebx,[rel length]
	mov [ebx],al
	inc byte [rel length]

.return:
	ret
;___________________________________








;举例：
;进来时esi指向"   love     123456 "
;出去时arg0="love"，[arg1]=123456，esi指向最后的空格
;本行全空会执行上一次的指令
;因为上一次取得的arg0和arg1没有被改变
;______________________________________
explainarg:
	mov esi,[consolehome+consolesize-8]		;;距离buffer开头多少
	add esi,consolehome+8					;;加上buffer开头地址


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
        mov qword [rel arg2],0		;先清理
        mov qword [rel arg2+8],0	;先清理
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
	ret
;____________________________________________
arg0:times 16 db 0		;本函数运行完的输出结果
arg1:times 16 db 0
arg2:times 16 db 0




;___________________________________
searchhere:
	lea esi,[rel arg0]
	cmp dword [esi],"powe"		;if [esi]=="powe"
	jne .skippoweroff
	cmp dword [esi+4],"roff"	;if [esi+4]=="roff"
	jne .skippoweroff
	cmp byte [esi+8],0		;if [esi+8]=0x20
	je poweroff
.skippoweroff:

	lea esi,[rel arg0]
	cmp dword [esi],"rebo"
	jne .skipreboot
	cmp word [esi+4],"ot"
	jne .skipreboot
	cmp byte [esi+6],0
	je reboot
.skipreboot:

	lea esi,[rel arg0]
	cmp dword [esi],"exit"
	jne .skipexit
	cmp byte [esi+4],0
	je poweroff
.skipexit:

	lea esi,[rel arg0]
	cmp dword [esi],"clea"
	jne .skipclear
	cmp byte [esi+4],'r'
	je clear
.skipclear:

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

	jmp f4event.notinhere
;_____________________________________




;找到就执行
;____________________________________
searchmemory:
	mov rax,[rel arg0]
	cmp rax,0x20
	jb .return

	mov esi,binhome
.search:
	cmp [esi],rax
	je .find
	add esi,0x10
	cmp esi,binhome+binsize
	jb .search
	jmp f4event.notinmemory

.find:
	mov rbx,[esi+8]
	mov [rel explainedarg0],rbx		;取得函数地址，保存在某处

	call checkandchangeline

	mov rbx,[rel explainedarg0]
	call rbx2string
	lea esi,[rel string]
	mov edi,[consolehome+consolesize-8]
	add edi,consolehome
	movsq
	movsq

	mov edi,[consolehome+consolesize-8]
	add edi,consolehome
	add edi,16+8
	lea rsi,[rel arg1]
	movsq
	movsq

	lea rdi,[rel arg1]
	call [rel explainedarg0]

	jmp f4event.scroll

.return:
	jmp f4event.notinmemory
;__________________________________________
explainedarg0:dq 0	;解释完是函数地址




;_______________________________________________
searchdisk:
	jmp f4event.notfound
;____________________________________________________




;__________________________________
test:
.cleanmemory:
	mov edi,0x2000000				;32m
	mov ecx,0x4000
	xor rax,rax
	rep stosd

	mov esi,binhome
.continue:
	cmp dword [esi],"load"
	je .load
	add esi,0x10
	cmp esi,binhome+binsize
	jb .continue
	jmp f4event.notfound
.load:
	mov rdx,[esi+8]
	lea rdi,[rel arg1]
	call rdx
.check:
	cmp dword [0x2000000],0
	je f4event.notfound
.execute:
	mov rax,0x2000000
	call rax
.return:
	jmp f4event.scroll
;__________________________________




;____________________________________
clear:
	mov edi,consolehome
	mov ecx,128*0x30
	xor rax,rax
	rep stosb

	mov edi,consolehome
	mov rax,"[   /]$ "
	stosq

	mov dword [consolehome+consolesize-8],0
	mov dword [rel length],8

	ret
;_______________________________________




;_________________________
ls:
	call checkandchangeline		;get new edi
	mov edi,[consolehome+consolesize-8]
	add edi,consolehome
	mov esi,0x4c0000
	xor ecx,ecx
.continue:
	cmp dword [esi],0
	je f4event.scroll
	movsq
	add esi,0x18
	add edi,0x8
	inc ecx

	cmp ecx,0x200
	jae f4event.scroll
	test ecx,0x7
	jnz .continue
	call checkandchangeline
	mov edi,[consolehome+consolesize-8]
	add edi,consolehome
	jmp .continue
;_________________________




;_______________________________
cast:
	lea esi,[rel arg1]
	call string2data
	call [rel value]
	jmp f4event.scroll
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
	jmp f4event.scroll
;_______________________________




;_______________________________________
outport:
	mov dx,[rel arg1]
	mov al,[rel arg2]
	out dx,al
	jmp f4event.scroll
inport:
	mov dx,[rel arg1]
	in al,dx
	jmp f4event.scroll
;________________________________________




;_________________________________
checkandchangeline:
	cmp dword [consolehome+consolesize-8],0x80*47
	jae .move

	add dword [consolehome+consolesize-8],128	;no:line+1
	jmp .return

.move:				;yes:move
	push rsi
	push rcx
	mov esi,consolehome+0x80
	mov edi,consolehome
	mov ecx,128*0x30
	cld
	rep movsb
	pop rcx
	pop rsi

.return:
	ret					;now line=a blank line
;____________________________________




;r8=arg0,r9=arg1,r10=arg2........
;_______________________________________________
say:
	mov rdi,[consolehome+consolesize-8]		;;距离buffer开头多少
	add rdi,consolehome+8					;;加上buffer开头地址
	mov rsi,r8
	mov cx,0x80
.continue:
	cmp byte [rsi],0
	je .return
	cmp byte [rsi],0xa
	jne .normalchar
	call checkandchangeline
.normalchar:
	movsb
	loop .continue

.return:
	ret
;___________________________________________




;____________________________________________________
welcomemessage:db "welcome to my brain"
dirtyword: db "wozhenshirilegoua"