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

	lea r8,[rel welcomemessage]
	call machinesay
	lea rdi,[rel welcomemessage]
	xor rax,rax
	mov rcx,0x80-5
	stosb
	lea r8,[rel userinput]
	call say


	mov rax,"xxxxxxxx"
	mov [consolehome+consolesize-0x20],rax
	mov rax,"yyyyyyyy"
	mov [consolehome+consolesize-0x10],rax

	ret
;____________________________________________________








;___________________________________________
f4showall:
	mov esi,[consolehome+consolesize-8]
	cmp esi,0x30*0x80					;buffer里面有48行吗
	jb .makeitzero
.makeitenough:							;显示区为:[当前位置-47行,当前位置]
	sub esi,0x2f*0x80
	jmp .esidone
.makeitzero:							;显示区为:[第0行,第47行]
	xor esi,esi
.esidone:
	add esi,consolehome					;前面算出的只是偏移

	mov edi,0x1c00000					;显示区在哪
	mov dword [rel .loopy],0
.continuey:
	mov dword [rel .loopx],0
.continuex:
	lodsb
	push rsi
	call char
	pop rsi
	inc byte [rel .loopx]
	cmp byte [rel .loopx],0x80
	jb .continuex

	add rdi,1024*4*15					;到下一行啦
	inc byte [rel .loopy]
	cmp byte [rel .loopy],0x30
	jb .continuey

	mov rbp,0x1c00000
	jmp writescreen
;________________________________________
.loopy:dd 0
.loopx:dd 0




;找到内存里哪一个字节对应屏幕上(x,y)坐标
;_________________________________________
f4showone:
	;int3
	mov edx,[consolehome+consolesize-8]	;edx=y*0x80
	mov ecx,[rel f4change]			;ecx=x

	lea esi,[ecx+edx]
	add esi,consolehome
	mov al,[esi]				;al=that byte
	;mov al,'@'

	cmp edx,0x30*0x80			;buffer里面有48行吗
	jae .atlastline

.atmiddle:									;48行以内在屏幕中间
	mov r10,rdx
	shr r10,7-4				;upy=y*16=edx/0x80*16
	lea r11,[r10+16]			;downy

	mov rdi,r10
	shl edi,10+2				;yoffset=upy*1024*4
	mov rbx,rcx
	shl rbx,3+2				;xoffset=leftx
	add edi,ebx
	add edi,0x1c00000
	call char

	jmp .ok

.atlastline:								;超过48行在最底下一行
	mov r10,768-16			;upy
	lea r11,[r10+16]		;downy

	mov edi,ecx
	shl edi,2+3								;总共8个点，每个点4byte
	add edi,0x1c00000+1024*4*16*47			;显示区在哪
	call char


.ok:
	mov r8,[rel f4change]	;leftx
	shl r8,3
	lea r9,[r8+8]			;rightx

	mov rbp,0x1c00000
	jmp updatescreen
;__________________________________________




;________________________________________
f4show:
	cmp dword [rel f4change],0x80			;小于0x80吗

	jb f4showone							;那就只要变一个字节不用全刷上屏幕

	jmp f4showall							;否则全部刷上屏幕
;________________________________________
f4change:dq 0xffff








;_______________________________________________
f4event:
	cmp al,0x80
	jae .return




.enter:
	cmp al,0x1c
	jne .notenter
.convert:
	mov r8,[consolehome+consolesize-8]		;距离buffer开头多少
	add r8,consolehome+5			;加上buffer开头地址
	call buf2arg

.startsearch:
.here:
	mov dword [rel failsignal],0
	call searchhere
	cmp dword [rel failsignal],0x11111111
	jne .successreturn
.memory:
	mov dword [rel failsignal],0
	call searchmemory
	cmp dword [rel failsignal],0x11111111
	jne .successreturn
.disk:
	;mov dword [rel failsignal],0
	;call searchdisk
	;cmp dword [rel failsignal],0x11111111
	;jne .successreturn

.notfound:							;哪都找不到就不找了，报告给用户如下
	lea r8,[rel notfoundmsg]		;notfound:
	call machinesay
	lea r8,[rel arg0msg]			;arg0=
	call say
	lea r8,[rel arg1msg]			;arg1=
	call say
	lea r8,[rel arg2msg]			;arg2=
	call say
	lea r8,[rel huanhang]
	call say
.successreturn:
	mov qword [rel failsignal],0
	lea r8,[rel userinput]			;user:
	call say

	mov qword [rel f4change],0xffff
	ret
.notenter:




.backspace:
	cmp al,0x0e
	jne .notbackspace
	cmp byte [rel length],5			;已经清理干净了(前面那个是"user:")
	jna .return

	mov ebx,[consolehome+consolesize-8]
	add ebx,consolehome
	dec byte [rel length]
	add ebx,[rel length]
	mov byte [ebx],0				;清掉被删的那个字节

	mov al,[rel length]
	movzx rax,al
	mov [rel f4change],rax		;告诉显示者我改了哪个字节
	;int3

	ret
.notbackspace:




.other:
	cmp byte [rel length],128
	jae .return

	call scan2anscii
	mov ebx,[consolehome+consolesize-8]
	add ebx,consolehome
	add ebx,[rel length]
	mov [ebx],al					;新增一个字节

	mov al,[rel length]
	movzx rax,al
	mov [rel f4change],rax		;告诉显示者我改了哪个字节

	inc byte [rel length]




.return:
	ret
;___________________________________
failsignal:dq 0








;_____________________________________________
length:dq 5
notfoundmsg:
	db "notfound->",0
userinput:
	db "user:"
welcomemessage:
	db "################"
	db "  welcome  into "
	db "  my  brain     "
	db "################"
huanhang:
	db 0xa,0
padding:
	times 0x80-(padding-userinput) db 0
;__________________________________________
