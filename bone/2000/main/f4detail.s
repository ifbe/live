%define binhome 0x6000
%define binsize 0x1000
%define consolehome 0xc00000
%define consolesize 0x100000
[bits 64]




;___________________________________
searchhere:
	lea esi,[rel arg0]

	cmp dword [esi],"powe"		;if( ( [esi]=="powe" )
	jne .skippoweroff
	cmp dword [esi+4],"roff"	;&& ( [esi+4]=="roff" )
	jne .skippoweroff
	cmp byte [esi+8],0			;&& ( [esi+8]=0x20 ) )
	je poweroff
.skippoweroff:

	cmp dword [esi],"rebo"		;else if
	jne .skipreboot
	cmp word [esi+4],"ot"
	jne .skipreboot
	cmp byte [esi+6],0
	je reboot
.skipreboot:

	cmp dword [esi],"exit"
	jne .skipexit
	cmp byte [esi+4],0
	je poweroff
.skipexit:

	cmp dword [esi],"clea"
	jne .skipclear
	cmp byte [esi+4],'r'
	je clear
.skipclear:

	cmp dword [esi],"writ"
	jne .skipwrite
	cmp byte [esi+4],'e'
	je write
.skipwrite:

	cmp dword [esi],"read"
	je read

	cmp dword [esi],"call"
	je cast

	cmp dword [esi],"jump"
	je jump

	cmp dword [esi],"out"
	je outport

	cmp word [esi],"in"
	je inport

	cmp dword [esi],"int"
	je enterinterrupt

	cmp word [esi],"ls"
	je ls

	cmp dword [esi],"test"
	je test

	cmp dword [esi],"time"
	je printtime

	jmp f4event.notinhere
;_____________________________________




;找到就执行
;____________________________________
searchmemory:
	cmp qword [rel arg0],0x20
	jb f4event.notinmemory			;不是anscii的函数名肯定不对

	mov esi,binhome
.search:
	cmp [esi],rax
	je .find
	add esi,0x10
	cmp esi,binhome+binsize
	jb .search

.notfound:
	jmp f4event.notinmemory

.find:
	mov rbx,[esi+8]
	mov [rel explainedarg0],rbx		;取得函数地址，保存在某处

	call checkandchangeline

	mov r8,[rel explainedarg0]
	call data2string
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

.normalreturn:
	lea r8,[rel userinput]
	call say
	ret
;__________________________________________
explainedarg0:dq 0	;解释完是函数地址




;__________________________________________
searchdisk:
	jmp f4event.notindisk
;________________________________________




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
	jmp f4event.notindisk
.load:
	mov rdx,[esi+8]
	lea rdi,[rel arg1]
	call rdx
.check:
	cmp dword [0x2000000],0
	je f4event.notindisk
.execute:
	mov rax,0x2000000
	call rax

.return:
	lea r8,[rel userinput]
	call say
	ret
;__________________________________








;____________________________________
clear:
	ret
;_______________________________________




;_________________________________________
write:
	ret
;_________________________________________




;________________________________________
read:
	lea r8,[rel arg1]
	call string2data
	ret
;_________________________________________




;_______________________________
cast:
	lea r8,[rel arg1]
	call string2data
	call [rel value]
.return:
	lea r8,[rel userinput]
	call say
	ret
;_______________________________




;_______________________________
jump:
	lea r8,[rel arg1]
	call string2data
	jmp [rel value]
;_______________________________




;_______________________________
enterinterrupt:
	mov qword [rel arg0],0
	int3
.return:
	lea r8,[rel userinput]
	call say
	ret
;_______________________________




;_______________________________________
outport:
	lea r8,[rel arg1]
	call string2data
	mov rax,[rel value]
	mov [rel arg1],rax

	lea r8,[rel arg2]
	call string2data
	mov rax,[rel value]
	mov [rel arg2],rax

.out8:
	mov dx,[rel arg1]
	mov al,[rel arg2]
	out dx,al
	jmp .return
.out16:
.out32:
.return:
	lea r8,[rel userinput]
	call say
	ret
;______________________________________




;________________________________________
inport:
	lea r8,[rel arg1]
	call string2data
	mov rax,[rel value]
	mov [rel arg1],rax
	
	mov dx,[rel arg1]
	in al,dx
	mov ah,al

	and ax,0x0ff0
	shr al,4

	add ax,0x3030

	cmp al,0x3a
	jb .skiphigh
	add al,0x7
.skiphigh:

	cmp ah,0x3a
	jb .skiplow
	add ah,0x7
.skiplow:

	mov [rel gottenmsg],ax
	lea r8,[rel gottenmsg]
	call machinesay
	lea r8,[rel huanhang]
	call machinesay

.return:
	lea r8,[rel userinput]
	call say
	ret
;________________________________________
gottenmsg:dq 0




;_________________________
ls:
	mov edi,[consolehome+consolesize-8]
	add edi,consolehome
	mov esi,0x4c0000
	xor ecx,ecx
.continue:
	cmp dword [esi],0
	je .return
	movsq
	add esi,0x18
	add edi,0x8
	inc ecx

	cmp ecx,0x200
	jae .return
	test ecx,0x7
	jnz .continue
	call checkandchangeline
	mov edi,[consolehome+consolesize-8]
	add edi,consolehome
	jmp .continue
.return:
	lea r8,[rel userinput]
	call say
	ret
;_________________________




;________________________________________
printtime:
	lea r8,[rel time]
	call machinesay
	lea r8,[rel huanhang]
	call say
.return:
	lea r8,[rel userinput]
	call say
	ret
;________________________________________
















;_________________________________
checkandchangeline:
	cmp dword [consolehome+consolesize-8],0x80*47
	jae .move

	add dword [consolehome+consolesize-8],128	;no:line+1
	jmp .return

.move:				;yes:move
	mov esi,consolehome+0x80
	mov edi,consolehome
	mov ecx,128*0x30
	cld
	rep movsb

.return:
	mov qword [rel length],0
	ret					;now line=a blank line
;____________________________________




;r8=arg0,r9=arg1,r10=arg2........
;_______________________________________________
machinesay:
	mov rdi,[consolehome+consolesize-8]	;距离buffer开头多少
	add rdi,consolehome			;加上buffer开头地址

	lea rsi,[rel senderinfomation]
	mov ecx,0x20
	mov [rel length],ecx
	rep movsb

say:
	mov rdi,[consolehome+consolesize-8]	;距离buffer开头多少
	add rdi,consolehome			;加上buffer开头地址
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
senderinfomation:
time:
	dq "20150804","220733"
name:
	dq ":system:",0




;_____________________________________________
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
