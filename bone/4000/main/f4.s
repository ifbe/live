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
f4show:
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

	jmp writescreen4
;________________________________________
looptimes:dd 0
length:dq 5








;_______________________________________________
f4event:
	cmp al,0x80
	jae .return




.backspace:
	cmp al,0x0e
	jne .notbackspace
	cmp byte [rel length],5
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
failsignal:dq 0








;_____________________________________________
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