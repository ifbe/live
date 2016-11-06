[bits 64]
startoflocalapic:




;______________检查是否有apic,没有就啥都不干______________
detectlocalapic:
	mov eax,1
	cpuid
	bt edx,9
	jnc endoflocalapic
;___________________________________________________




;_______找到了apic,如果地址不是0xfee00000还是罢工________
findlocalapic:
	mov rcx,0x1b
	rdmsr
	;bts eax,11            ;enable lapic
	;wrmsr
	and eax,0xfffff000
	cmp eax,0xfee00000
	jb endoflocalapic
;__________________________________________________




;___________________一切正常,开始初始化_____________________
localapicinitialization:
	mov edi,0xfee00000

;task priority
;[+80]=0
	xor eax,eax
	mov [edi+0x80],eax    ;(cr8=0<<4)

;disable timer interrupt
;[+320] = 10000
	mov eax,0x10000
	mov [edi+0x320],eax

;disable performance counter interrupt
;[+340]=10000
	mov eax,0x10000
	mov [edi+0x340],eax

;disable local interrupt 0(normal external interrupt)
;[+350]=8700f
	mov eax,0x8700
	mov [edi+0x350],eax

;disable local interrupt 1(normal nmi processing)
;[+360]=400
	mov eax,0x400
	mov [edi+0x360],eax

;disable error interrupt
;[+370]=10000
	mov eax,0x10000
	mov [edi+0x370],eax

;disable spurious vector+enable apic
;[+f0]=10f
	mov eax,0x170
	mov [edi+0xf0],eax
;_________________________________________
endoflocalapic:








startofioapic:




;____________________________________
disable8259:
	mov al, 0xff
	out 0xa1, al
	out 0x21, al
;_________________________________________




;_____________mask all interrupt__________
	mov edi,0xfec00000
	mov eax,0x10
maskall:
	mov [edi],eax
	inc eax
	mov dword [edi],0x10000

	mov [edi],eax
	inc eax
	mov dword [edi+0x10],0

	cmp eax,0x10+2*24
	jb maskall
;____________________________________


jmp endofioapic




paddingofioapic:
times 0x200-(paddingofioapic-startoflocalapic) db 0


endofioapic:
