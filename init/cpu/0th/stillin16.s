BITS 16
startof16:

;_________________环境设置__________________
	cli
	xor ax,ax    ; Set up segment registers.
	mov ds,ax
	mov es,ax
	mov ss,ax    ; Set up stack so that it starts below Main.
	mov sp,0x7c00
;___________________________________________


;_______________________________________
	;clear [2000,7fff]

	xor eax,eax
	mov di,0x2000
	mov cx,0x1800
	cld
	rep stosd
;__________________________________________


;____________________________
CheckCPU:    ; Check whether CPUID is supported or not.

	pushfd    ; Get flags in EAX register.

	pop eax
	mov ecx, eax
	xor eax, 0x200000
	push eax
	popfd

	pushfd
	pop eax
	xor eax, ecx
	shr eax, 21
	and eax, 1
	push ecx
	popfd

	test eax, eax		;If EAX now contains 0,
	jz no64			;CPUID isn't supported.

	mov eax, 0x80000000
	cpuid
	cmp eax, 0x80000001		;extended function 0x80000001
	jb no64			;not support, long mode not supported.

	mov eax, 0x80000001
	cpuid
	test edx, 1 << 29		;Test if the LM-bit, is set or not.
	jz no64			;If not Long mode not supported.

	jmp supportlongmode
;_____________________________________________


;________________________________
no64:
	mov ax,0xb800
	mov es,ax
	mov di,0
	mov ah,0xf0

	mov al,'n'
	stosw
	mov al,'o'
	stosw
	mov al,'6'
	stosw
	mov al,'4'
	stosw
;______________________________


;______________________________
die:hlt
    jmp die
;_____________________________




;____________read cmos/disable nmi_____________
supportlongmode:

	mov di,0x500
cmos:
	lea ax,[di-0x480]
	out 0x70,al
	in al,0x71
	stosb
	cmp di,0x580
	jb cmos
;_____________________________________________




;____________int15 e820 memory detect___________
	mov di,0x2000
	xor ebx,ebx
e820:
	mov eax,0xe820
	mov ecx,24
	mov edx,0x534d4150
	int 0x15
	jc e820finish
	cmp eax,0x534d4150
	jne e820finish
	add di,0x20
	cmp di,0x3000
	jae e820finish
	cmp ebx,0
	jne e820

e820finish:
;_____________________________________________




;______________列出vesa模式________________
	mov cx,0x101
	mov di,0x3020

listresolution:
	mov ax,0x4f01
	int 0x10         ;changed:ax
	add di,0x20
	inc cx
	cmp cx,0x180
	jb listresolution
;_____________________________________________


	jmp endof16


paddingof16:
times 0x100-(paddingof16-startof16) db 0

endof16:
