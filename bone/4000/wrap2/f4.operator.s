[bits 64]




;_______________________________
enterinterrupt:
	mov qword [rel arg0],0
	int3

	ret
;_______________________________




;_______________________________
cast:
	lea r8,[rel arg1]
	call string2data
	call [rel value]

	ret
;_______________________________




;____________________________________
jump:
	lea r8,[rel arg1]
	call string2data
	jmp [rel value]
;___________________________________




;__________________________________________
getrdx:
	lea r8,[rel arg1]
	call string2data
	mov rdx,[rel value]
	ret


rdxrax:
	lea r8,[rel arg1]
	call string2data
	mov rax,[rel value]
	mov [rel arg1],rax

	lea r8,[rel arg2]
	call string2data
	mov rax,[rel value]
	mov [rel arg2],rax

	mov rdx,[rel arg1]
	mov rax,[rel arg2]
	ret
;_________________________________________




;_________________________________________
write:
	call rdxrax
	mov [rdx],al
	ret


write16:
	call rdxrax
	mov [rdx],ax
	ret


write32:
	call rdxrax
	mov [rdx],eax
	ret
;_________________________________________




;________________________________________
read:
	call getrdx

	movzx r8,byte [rdx]
	lea r9,[rel string]
	call data2hexstring

	lea r8,[rel string+0xe]
	call machinesay

	ret


read16:
	call getrdx

	movzx r8,word [rdx]
	lea r9,[rel string]
	call data2hexstring

	lea r8,[rel string+0xc]
	call machinesay

	ret


read32:
	call getrdx

	mov r8d,dword [rdx]
	lea r9,[rel string]
	call data2hexstring

	lea r8,[rel string+0x8]
	call machinesay

	ret
;_________________________________________




;________________out dx,al/ax/eax___________
out8:
	call rdxrax
	out dx,al
	ret


out16:
	call rdxrax
	out dx,ax
	ret


out32:
	call rdxrax
	out dx,eax
	ret
;______________________________________




;_________________in al/ax/eax,dx____________
in8:
	call getrdx
	in al,dx

	movzx r8,al
	lea r9,[rel string]
	call data2hexstring

	lea r8,[rel string+0xe]
	call machinesay

	ret


in16:
	call getrdx
	in ax,dx

	movzx r8,ax
	lea r9,[rel string]
	call data2hexstring

	lea r8,[rel string+0xc]
	call machinesay

	ret


in32:
	call getrdx
	in eax,dx

	mov r8d,eax
	lea r9,[rel string]
	call data2hexstring

	lea r8,[rel string+0x8]
	call machinesay

	ret
;________________________________________
gottenmsg:dq 0




;___________________________________________
timestamp:
	rdtsc

	mov r8,rdx
	shl r8,32
	shl rax,32
	shr rax,32
	add r8,rax
	lea r9,[rel string]
	call data2hexstring

	lea r8,[rel string]
	call machinesay

	ret
;___________________________________________




;____________________________________________
readmsr:
	call getrdx
	mov rcx,rdx
	rdmsr

	shl rdx,32
	mov r8,rdx
	shl rax,32
	shr rax,32
	add r8,rax
	lea r9,[rel string]
	call data2hexstring

	lea r8,[rel string]
	call machinesay

	ret
;____________________________________________




;____________________________________________
writemsr:
	call rdxrax

	mov rcx,rdx
	mov rdx,rax
	shr rdx,32
	shl rax,32
	shr rax,32
	wrmsr

	ret
;____________________________________________




;_______________________________________
printcpuid:
	call getrdx
	mov rax,rdx
	cpuid

	mov [rel preserverax],rax
	mov [rel preserverbx],rbx
	mov [rel preservercx],rcx
	mov [rel preserverdx],rdx

	mov r8,rax
	lea r9,[rel string]
	call data2hexstring
	lea r8,[rel string+8]
	call machinesay

	mov r8,[rel preserverbx]
	lea r9,[rel string]
	call data2hexstring
	lea r8,[rel string+8]
	call machinesay

	mov r8,[rel preservercx]
	lea r9,[rel string]
	call data2hexstring
	lea r8,[rel string+8]
	call machinesay

	mov r8,[rel preserverdx]
	lea r9,[rel string]
	call data2hexstring
	lea r8,[rel string+8]
	call machinesay

	ret
;_______________________________________
preserverax:dq 0
preserverbx:dq 0
preservercx:dq 0
preserverdx:dq 0;入:r8,r9,r10,r11......




;出:r8,r9,r10,r11......
;变:rax,r8,r9 + data2hexstring
;_____________________________________
gettime:
	xor rax,rax				;must clean

	mov al,0x32				;[63:56]:centry
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,9				;[55,48]:year
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,8				;[47,40]:month
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,7				;[39,32]:day
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,4				;[31,24]:hour
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,2				;[23,16]:minute
	out 0x70,al
	in al,0x71

	shl rax,8
	mov al,0				;[15,8]:second
	out 0x70,al
	in al,0x71
	shl rax,8

	mov r8,rax
	lea r9,[rel time]
	call data2hexstring
	mov word [rel time+0xe],0x2020		;milesecond

	ret




printtime:
	lea r8,[rel time]
	call machinesay				;这个函数会自动更新时间

	ret
;________________________________________




;___________________________________________
print8254:
;	mov r8,[rel innercount]
;	lea r9,[rel string]
;	call data2decimalstring
;
;	lea r8,[rel string]
;	call machinesay

	ret
;___________________________________________




;____________________________________
clear:
	ret
;_____________________________________




;___________________________________________
cpufreq:
;	rdtsc
;	mov [rel timestamplow],eax
;	mov [rel timestamphigh],edx
;
;	mov r8,1000/16				;		/16
;	call delay
;
;	rdtsc
;	sub eax,[rel timestamplow]
;	sub edx,[rel timestamphigh]
;	shl rdx,32
;	add edx,eax
;	shl rdx,4					;		*16
;
;	mov r8,rdx
;	lea r9,[rel string]
;	call data2decimalstring
;
;	lea r8,[rel string]
;	call machinesay
	ret
;___________________________________________
timestamplow:dd 0
timestamphigh:dd 0




;___________________________________________
memfreq:
	ret
;___________________________________________