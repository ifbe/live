%define acpihome 0x40000
%define pcihome 0x60000
%define binhome 0x70000

%define ahcihome 0x400000
%define xhcihome 0x600000
%define usbhome 0x700000

%define diskhome 0x800000
%define fshome 0x900000
%define dirhome 0xa00000
%define filehome 0xb00000

%define consolehome 0xc00000
%define consolesize 0x100000
[bits 64]




;__________________________________
lsacpi:
	mov edi,[consolehome+consolesize-8]
	cmp edi,consolesize-0x30*0x80
	jb .normally
	xor edi,edi
	mov [consolehome+consolesize-8],edi
.normally:
	add edi,consolehome

	mov dword [rel acpidest],edi
	mov dword [rel acpioffset],0
	cld




;while(1)
;{
.continue:

	;if(acpioffset>0x1000)break;
	mov eax,[rel acpioffset]
	cmp eax,0x1000				;不直接比，因为下面用到eax
	jae .return

	;if(this==0)break;
	mov esi,acpihome
	add esi,eax
	cmp dword [esi],0					;[acpihome+offset]=0?
	je .return

	;put
	mov esi,[rel acpioffset]			;[acpihome+offset+0]
	add esi,acpihome
	mov edi,[rel acpidest]
	movsq

	mov esi,[rel acpioffset]			;[acpihome+offset+8]
	add esi,acpihome+8
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel acpidest]
	add edi,0x10
	stosq

	;next
	add dword [rel acpidest],0x80
	add dword [rel acpioffset],0x10
	jmp .continue
;}




.return:
	mov eax,[rel acpidest]
	sub eax,consolehome
	mov [consolehome+consolesize-8],eax
	ret
;__________________________________
acpidest:dq 0
acpioffset:dq 0




;__________________________________
lspci:
	mov edi,[consolehome+consolesize-8]
	cmp edi,consolesize-0x30*0x80
	jb .normally
	xor edi,edi
	mov [consolehome+consolesize-8],edi
.normally:
	add edi,consolehome

	mov dword [rel pcidest],edi
	mov dword [rel pcioffset],0
	cld




;while(1)
;{
.continue:

	;if(pcioffset>0x1000)break;
	mov eax,[rel pcioffset]
	cmp eax,0x1000				;不直接比，因为下面用到eax
	jae .return

	;if(this==0)break;
	mov esi,pcihome
	add esi,eax
	cmp dword [esi],0					;[pcihome+offset]=0?
	je .return

	;put
	mov esi,[rel pcioffset]			;[pcihome+offset]
	add esi,pcihome+0
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel pcidest]
	stosq

	mov esi,[rel pcioffset]			;[pcihome+offset+8]
	add esi,pcihome+8
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel pcidest]
	add edi,0x10
	stosq

	mov esi,[rel pcioffset]			;[pcihome+offset+0x10]
	add esi,pcihome+0x10
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel pcidest]
	add edi,0x20
	stosq

	mov esi,[rel pcioffset]			;[pcihome+offset+0x10]
	add esi,pcihome+0x18
	mov edi,[rel pcidest]
	add edi,0x30
	movsq

	;next
	add dword [rel pcidest],0x80
	add dword [rel pcioffset],0x40
	jmp .continue
;}




.return:
	mov eax,[rel pcidest]
	sub eax,consolehome
	mov [consolehome+consolesize-8],eax
	ret
;__________________________________
pcidest:dq 0
pcioffset:dq 0




;__________________________________
lsbin:
	mov edi,[consolehome+consolesize-8]
	cmp edi,consolesize-0x30*0x80
	jb .normally
	xor edi,edi
	mov [consolehome+consolesize-8],edi
.normally:
	add edi,consolehome

	mov dword [rel bindest],edi
	mov dword [rel binoffset],0
	cld




;while(1)
;{
.continue:

	;if(binoffset>0x1000)break;
	mov eax,[rel binoffset]
	cmp eax,0x1000				;不直接比，因为下面用到eax
	jae .return

	;if(this==0)break;
	mov esi,binhome
	add esi,eax
	cmp dword [esi],0					;[binhome+offset]=0?
	je .return

	;put
	mov esi,[rel binoffset]			;[binhome+offset+0]
	add esi,binhome
	mov edi,[rel bindest]
	movsq

	mov esi,[rel binoffset]			;[binhome+offset+8]
	add esi,binhome+8
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel bindest]
	add edi,0x10
	stosq

	;next
	add dword [rel bindest],0x80
	add dword [rel binoffset],0x10
	jmp .continue
;}




.return:
	mov eax,[rel bindest]
	sub eax,consolehome
	mov [consolehome+consolesize-8],eax
	ret
;__________________________________
bindest:dq 0
binoffset:dq 0




;__________________________________
lsahci:
	mov edi,[consolehome+consolesize-8]
	cmp edi,consolesize-0x30*0x80
	jb .normally
	xor edi,edi
	mov [consolehome+consolesize-8],edi
.normally:
	add edi,consolehome

	mov dword [rel ahcidest],edi
	mov dword [rel ahcioffset],0
	cld




;while(1)
;{
.continue:

	;if(ahcioffset>0x1000)break;
	mov eax,[rel ahcioffset]
	cmp eax,0x1000				;不直接比，因为下面用到eax
	jae .return

	;if(this==0)break;
	mov esi,ahcihome
	add esi,eax
	cmp dword [esi],0					;[ahcihome+offset]=0?
	je .return

	;put
	mov esi,[rel ahcioffset]			;[ahcihome+offset]
	add esi,ahcihome
	mov edi,[rel ahcidest]
	movsq

	mov esi,[rel ahcioffset]			;[ahcihome+offset+8]
	add esi,ahcihome+8
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel ahcidest]
	add edi,0x10
	stosq

	mov esi,[rel ahcioffset]			;[ahcihome+offset+0x10]
	add esi,ahcihome+0x10
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel ahcidest]
	add edi,0x20
	stosq

	mov esi,[rel ahcioffset]			;[ahcihome+offset+0x18]
	add esi,ahcihome+0x18
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel ahcidest]
	add edi,0x30
	stosq

	;next
	add dword [rel ahcidest],0x80
	add dword [rel ahcioffset],0x40
	jmp .continue
;}




.return:
	mov eax,[rel ahcidest]
	sub eax,consolehome
	mov [consolehome+consolesize-8],eax
	ret
;__________________________________
ahcidest:dq 0
ahcioffset:dq 0




;______________________________________
lsxhci:
	ret
;_______________________________________




;__________________________________
lsusb:
	mov edi,[consolehome+consolesize-8]
	cmp edi,consolesize-0x30*0x80
	jb .normally
	xor edi,edi
	mov [consolehome+consolesize-8],edi
.normally:
	add edi,consolehome

	mov dword [rel usbdest],edi
	mov dword [rel usboffset],0
	cld




;while(1)
;{
.continue:

	;if(usboffset>0x1000)break;
	mov eax,[rel usboffset]
	cmp eax,0x1000				;不直接比，因为下面用到eax
	jae .return

	;if(this==0)break;
	mov esi,usbhome
	add esi,eax
	cmp dword [esi],0				;[usbhome+offset]=0?
	je .return

	;put
	mov esi,[rel usboffset]			;[usbhome+offset]
	add esi,usbhome+0
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel usbdest]
	stosq

	mov esi,[rel usboffset]			;[usbhome+offset+8]
	add esi,usbhome+0x8
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel usbdest]
	add edi,0x10
	stosq

	mov esi,[rel usboffset]			;[usbhome+offset+0x10]
	add esi,usbhome+0x10
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel usbdest]
	add edi,0x20
	stosq

	mov esi,[rel usboffset]			;[usbhome+offset+0x18]
	add esi,usbhome+0x18
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel usbdest]
	add edi,0x30
	stosq

	mov esi,[rel usboffset]			;[usbhome+offset+0x20]
	add esi,usbhome+0x20
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel usbdest]
	add edi,0x40
	stosq

	;next
	add dword [rel usbdest],0x80
	add dword [rel usboffset],0x40
	jmp .continue
;}




.return:
	mov eax,[rel usbdest]
	sub eax,consolehome
	mov [consolehome+consolesize-8],eax
	ret
;__________________________________
usbdest:dq 0
usboffset:dq 0




;__________________________________
lsdisk:
	mov edi,[consolehome+consolesize-8]
	cmp edi,consolesize-0x30*0x80
	jb .normally
	xor edi,edi
	mov [consolehome+consolesize-8],edi
.normally:
	add edi,consolehome

	mov dword [rel diskdest],edi
	mov dword [rel diskoffset],0
	cld




;while(1)
;{
.continue:

	;if(diskoffset>0x1000)break;
	mov eax,[rel diskoffset]
	cmp eax,0x1000				;不直接比，因为下面用到eax
	jae .return

	;if(this==0)break;
	mov esi,diskhome
	add esi,eax
	cmp dword [esi],0					;[diskhome+offset]=0?
	je .return

	;put
	mov esi,[rel diskoffset]			;[diskhome+offset]
	add esi,diskhome
	mov edi,[rel diskdest]
	movsq

	mov esi,[rel diskoffset]			;[diskhome+offset+8]
	add esi,diskhome+8
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel diskdest]
	add edi,0x10
	stosq

	mov esi,[rel diskoffset]			;[diskhome+offset+0x10]
	add esi,diskhome+0x10
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel diskdest]
	add edi,0x20
	stosq

	mov esi,[rel diskoffset]			;[diskhome+offset+0x18]
	add esi,diskhome+0x18
	mov r8,[esi]
	lea r9,[rel string]
	call data2hexstring
	mov rax,[rel string+8]
	mov edi,[rel diskdest]
	add edi,0x30
	stosq

	;next
	add dword [rel diskdest],0x80
	add dword [rel diskoffset],0x40
	jmp .continue
;}




.return:
	mov eax,[rel diskdest]
	sub eax,consolehome
	mov [consolehome+consolesize-8],eax
	ret
;__________________________________
diskdest:dq 0
diskoffset:dq 0




;______________________________________
lsfs:
	ret
;_______________________________________




;_________________________
ls:
	mov edi,[consolehome+consolesize-8]
	cmp edi,consolesize-0x30*0x80
	jb .normally
	xor edi,edi
	mov [consolehome+consolesize-8],edi
.normally:
	add edi,consolehome

	mov esi,dirhome			;source
	xor ecx,ecx			;count




.continue:
	cmp dword [esi],0		;finish?
	je .return

	cmp ecx,0x200			;finish?
	jae .return

	movsq					;put
	add esi,0x18
	add edi,0x8

	inc ecx					;next
	jmp .continue




.return:
	add ecx,7
	shr ecx,3			;how many wrote
	shl ecx,7
	add [consolehome+consolesize-8],ecx
	ret
;_________________________
