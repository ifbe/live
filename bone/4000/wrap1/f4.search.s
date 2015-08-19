%define binhome 0x70000
%define binsize 0x1000

%define diskhome 0x100000
%define fshome 0x200000
%define filehome 0x300000

%define programhome 0x400000

%define consolehome 0xc00000
%define consolesize 0x100000
[bits 64]




;___________________________________
searchhere:
	lea esi,[rel arg0]

	cmp dword [esi],"exit"
	je poweroff

	cmp dword [esi],"powe"		;if( ( [esi]=="powe" )
	jne .skippoweroff
	cmp dword [esi+4],"roff"	;&& ( [esi+4]=="roff" ) )
	je poweroff
.skippoweroff:

	cmp dword [esi],"rebo"		;else if
	jne .skipreboot
	cmp word [esi+4],"ot"
	je reboot
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

	cmp dword [esi],"wwww"
	je write32
	cmp dword [esi],"ww"
	je write16
	cmp dword [esi],"writ"
	jne .skipwrite
	cmp byte [esi+4],'e'
	je write
.skipwrite:

	cmp dword [esi],"rrrr"
	je read32
	cmp dword [esi],"rr"
	je read16
	cmp dword [esi],"read"
	je read

	cmp dword [esi],"oooo"
	je out32
	cmp dword [esi],"oo"
	je out16
	cmp dword [esi],"out"
	je out8

	cmp dword [esi],"iiii"
	je in32
	cmp dword [esi],"ii"
	je in16
	cmp dword [esi],"in"
	je in8

	cmp dword [esi],"time"
	je printtime

	cmp dword [esi],"8254"
	je print8254

	cmp dword [esi],"clea"
	jne .skipclear
	cmp byte [esi+4],'r'
	je clear
.skipclear:

	cmp dword [esi],"cpuf"
	jne .skipcpufreq
	cmp dword [esi+4],"req"
	je cpufreq
.skipcpufreq:

	cmp dword [esi],"memf"
	jne .skipmemfreq
	cmp dword [esi+4],"req"
	je memfreq
.skipmemfreq:

	cmp word [esi],"ls"
	je ls

	cmp dword [esi],"test"
	je test

	mov dword [rel failsignal],0x11111111
	ret
;_____________________________________




;找到就执行
;____________________________________
searchmemory:
	mov rax,[rel arg0]
	cmp rax,0x20
	jb .failreturn			;连名字都没有,肯定不对

	mov esi,binhome
.search:
	cmp [esi],rax
	je .find
	add esi,0x10
	cmp esi,binhome+binsize
	jb .search

.failreturn:
	mov dword [rel failsignal],0x11111111
	ret




.find:
	mov r8,[esi+8]
	mov [rel addrtocall],r8		;暂时保存函数地址
	lea r9,[rel string]
	call hex2string

	lea r8,[rel string+8]
	call machinesay

	lea rdi,[rel arg1]			;往里传的第一个参数
	call [rel addrtocall]
	ret
;__________________________________________
addrtocall:dq 0	;解释完是函数地址




;__________________________________
test:
.cleanmemory:
	mov edi,programhome				;32m
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
	jmp .failreturn
.load:
	mov rdx,[esi+8]
	lea rdi,[rel arg1]
	call rdx
.check:
	cmp dword [programhome],0
	je .failreturn
.execute:
	mov rax,programhome
	call rax

.failreturn:
	mov dword [rel failsignal],0x11111111
	ret
;__________________________________




;_________________________
ls:
	mov esi,filehome
	xor ecx,ecx
.huanhang:
	mov edi,[consolehome+consolesize-8]
	add edi,consolehome
.continue:
	cmp dword [esi],0		;finish?
	je .return

	inc ecx
	cmp ecx,0x200			;finish?
	jae .return

	movsq				;put
	add esi,0x18
	add edi,0x8

	test ecx,0x7			;huanhang?
	jnz .continue

	call checkandchangeline
	jmp .huanhang

.return:
	call checkandchangeline
	ret
;_________________________
