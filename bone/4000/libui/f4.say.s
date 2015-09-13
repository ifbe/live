%define consolehome 0xc00000
%define consolesize 0x100000
[bits 64]




;____________________________________
checkandchangeline:
	mov qword [rel length],0

	cmp dword [consolehome+consolesize-8],consolesize-0x80*0x30
	jae .restart

.justadd:
	add dword [consolehome+consolesize-8],0x80
	ret

.restart:
	mov dword [consolehome+consolesize-8],0x80
	ret
;_________________________________




;r8=arg0,r9=arg1,r10=arg2........
;_______________________________________________
machinesay:
	push r8
	call gettime
	pop r8

	mov rdi,[consolehome+consolesize-8]		;距离buffer开头多少
	add rdi,consolehome						;加上buffer开头地址

	lea rsi,[rel time]						;时间
	movsq
	movsq

	lea rsi,[rel name]						;机器名
	movsq
	movsq

	mov qword [rel length],0x20					;总共写了0x20字节




say:
	mov rdi,[consolehome+consolesize-8]		;距离buffer开头多少
	add rdi,consolehome						;加上buffer开头地址
	add rdi,[rel length]

	mov rsi,r8
	mov ecx,[rel length]
.continue:
	cmp byte [rsi],0
	je .return

	cmp byte [rsi],0xa
	jne .normalchar

.nextline:
	call checkandchangeline
	inc rsi
	jmp .nextchar

.normalchar:
	movsb
	inc qword [rel length]

.nextchar:
	inc ecx
	cmp ecx,0x80
	jb .continue

.return:
	ret
;___________________________________________
time:
	dq "ccyymmdd","hhmmss  "
	db 0xa,0
name:
	dq "    syst","em:"
	db 0xa,0