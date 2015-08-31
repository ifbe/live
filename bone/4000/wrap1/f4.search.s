%define binhome 0x70000
%define binsize 0x1000
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
	jne .skipread
	cmp byte [esi+4],0
	je read
.skipread:

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
	jne .skipls
	cmp dword [esi+2],"pci"
	je lspci
	cmp dword [esi+2],"acpi"
	je lsacpi
	cmp dword [esi+2],"ahci"
	je lsahci
	cmp dword [esi+2],"usb"
	je lsusb
	cmp dword [esi+2],"disk"
	je lsdisk
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