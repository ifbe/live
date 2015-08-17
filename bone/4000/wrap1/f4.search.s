%define binhome 0x6000
%define binsize 0x1000
%define consolehome 0xc00000
%define consolesize 0x100000
[bits 64]




;___________________________________
searchhere:
	lea esi,[rel arg0]

	cmp dword [esi],"time"
	je printtime

	cmp dword [esi],"8254"
	je print8254

	cmp dword [esi],"clea"
	jne .skipclear
	cmp byte [esi+4],'r'
	je clear
.skipclear:

	cmp dword [esi],"exit"
	je poweroff

	cmp dword [esi],"powe"		;if( ( [esi]=="powe" )
	jne .skippoweroff
	cmp dword [esi+4],"roff"	;&& ( [esi+4]=="roff" ) )
	je .skippoweroff
.skippoweroff:

	cmp dword [esi],"rebo"		;else if
	jne .skipreboot
	cmp word [esi+4],"ot"
	je .skipreboot
.skipreboot:

	cmp dword [esi],"cpui"
	jne .skipcpuid
	cmp byte [esi+4],'d'
	je printcpuid
.skipcpuid:

	cmp word [esi],"rd"
	jne .skiprd
	cmp dword [esi+2],"tsc"
	je timestamp
	cmp dword [esi+2],"msr"
	je readmsr
.skiprd:

	cmp word [esi],"wr"
	jne .skipwr
	cmp dword [esi+2],"msr"
	je writemsr
.skipwr:

	cmp dword [esi],"int"
	je enterinterrupt

	cmp dword [esi],"call"
	je cast

	cmp dword [esi],"jump"
	je jump

	cmp dword [esi],"writ"
	jne .skipwrite
	cmp byte [esi+4],'e'
	je write
.skipwrite:
	cmp dword [esi],"ww"
	je write16
	cmp dword [esi],"wwww"
	je write32

	cmp dword [esi],"read"
	je read
	cmp dword [esi],"rr"
	je read16
	cmp dword [esi],"rrrr"
	je read32

	cmp dword [esi],"out"
	je out8
	cmp dword [esi],"oo"
	je out16
	cmp dword [esi],"oooo"
	je out32

	cmp dword [esi],"in"
	je in8
	cmp dword [esi],"ii"
	je in16
	cmp dword [esi],"iiii"
	je in32

	cmp word [esi],"ls"
	je ls

	cmp dword [esi],"test"
	je test

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
	lea r9,[rel string]
	call hex2string
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