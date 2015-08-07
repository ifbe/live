[bits 64]




;_______________________________
enterinterrupt:
	mov qword [rel arg0],0
	int3
.return:
	lea r8,[rel userinput]
	call say
	ret
;_______________________________




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
	jmp writereturn


write16:
	call rdxrax
	mov [rdx],ax
	jmp writereturn


write32:
	call rdxrax
	mov [rdx],eax
	jmp writereturn


writereturn:
	lea r8,[rel userinput]
	call say
	ret
;_________________________________________




;________________________________________
read:
	call getrdx

	movzx r8,byte [rdx]
	call data2string

	lea r8,[rel string+0xe]
	call machinesay

	jmp readreturn


read16:
	call getrdx

	movzx r8,word [rdx]
	call data2string

	lea r8,[rel string+0xc]
	call machinesay

	jmp readreturn


read32:
	call getrdx

	mov r8d,dword [rdx]
	call data2string

	lea r8,[rel string+0x8]
	call machinesay


readreturn:
	lea r8,[rel userinput]
	call say

	ret
;_________________________________________




;________________out dx,al/ax/eax___________
out8:
	call rdxrax
	out dx,al
	jmp outreturn


out16:
	call rdxrax
	out dx,ax
	jmp outreturn


out32:
	call rdxrax
	out dx,eax


outreturn:
	lea r8,[rel userinput]
	call say
	ret
;______________________________________




;_________________in al/ax/eax,dx____________
in8:
	call getrdx
	in al,dx

	movzx r8,al
	call data2string

	lea r8,[rel string+0xe]
	call machinesay

	jmp inreturn


in16:
	call getrdx
	in ax,dx

	movzx r8,ax
	call data2string

	lea r8,[rel string+0xc]
	call machinesay

	jmp inreturn


in32:
	call getrdx
	in eax,dx

	mov r8d,eax
	call data2string

	lea r8,[rel string+0x8]
	call machinesay


inreturn:
	lea r8,[rel userinput]
	call say

	ret
;________________________________________
gottenmsg:dq 0




;____________________________________
clear:
	ret
;_______________________________________




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