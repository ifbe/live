%define binhome 0x70000
%define binsize 0x1000
[bits 64]




;_______________________________________________
f4explain:

.convert:
	mov r8,[consolehome+consolesize-8]		;距离buffer开头多少
	add r8,consolehome+5			;加上buffer开头地址
	call buf2arg
	call checkandchangeline

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
		;lea r8,[rel arg0msg]			;arg0=
		;call say
		;lea r8,[rel arg1msg]			;arg1=
		;call say
		;lea r8,[rel arg2msg]			;arg2=
		;call say
		;lea r8,[rel huanhang]
		;call say

.successreturn:
	mov qword [rel failsignal],0
	lea r8,[rel userinput]			;user:
	call say

	ret
;__________________________________________________
failsignal:dq 0




;____________________________________________
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
		cmp dword [esi+4],"e32"
		je write32
		cmp dword [esi+4],"e16"
		je write16
		cmp dword [esi+4],"e"
		je write8
.skipwrite:

	cmp dword [esi],"rrrr"
	je read32
	cmp dword [esi],"rr"
	je read16
	cmp dword [esi],"read"
	jne .skipread
		cmp dword [esi+4],"32"
		je read32
		cmp dword [esi+4],"16"
		je read16
		cmp byte [esi+4],0
		je read8
.skipread:

	cmp dword [esi],"oooo"
	je out32
	cmp dword [esi],"oo"
	je out16
	cmp word [esi],"ou"
	jne .skipout
		cmp dword [esi+2],"t32"
		je out32
		cmp dword [esi+2],"t16"
		je out16
		cmp word [esi+2],"t"
		je out8
.skipout:

	cmp dword [esi],"iiii"
	je in32
	cmp dword [esi],"ii"
	je in16
	cmp dword [esi],"in32"
	je in32
	cmp dword [esi],"in16"
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
	jne .skipls
	cmp dword [esi+2],"acpi"		;40000
	je lsacpi
	cmp dword [esi+2],"pci"			;60000
	je lspci
	cmp dword [esi+2],"bin"			;70000
	je lsbin
	cmp dword [esi+2],"ahci"		;400000
	je lsahci
	cmp dword [esi+2],"xhci"		;600000
	je lsxhci
	cmp dword [esi+2],"usb"			;700000
	je lsusb
	cmp dword [esi+2],"disk"		;800000
	je lsdisk
	cmp dword [esi+2],"fs"			;900000
	je lsfs
	cmp byte [esi+2],0
	je ls
.skipls:

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
	call data2hexstring

	lea r8,[rel string+8]
	call machinesay

	lea rdi,[rel arg1]			;往里传的第一个参数
	lea rsi,[rel arg2]			;往里传的第一个参数
	lea rdx,[rel arg3]			;往里传的第一个参数
	lea rcx,[rel arg4]			;往里传的第一个参数
	lea r8,[rel arg5]			;往里传的第一个参数
	lea r9,[rel arg6]			;往里传的第一个参数
	call [rel addrtocall]
	ret
;__________________________________________
addrtocall:dq 0	;解释完是函数地址




;___________________________________________
searchdisk:
	ret
;___________________________________________