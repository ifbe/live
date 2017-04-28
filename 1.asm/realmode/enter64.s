%define bornhome 0x800
%define jmprax 0x810

%define gdtr 0x840
%define idtr 0x850
%define gdthome 0x3000

%define temppml4 0x4000
%define temppdpt 0x5000
%define temppd 0x6000
%define temppt 0x7000

[BITS 16]
startofenter64:




;_________________________________________________
;打开A20地址线
	in al, 92h
	or al, 2
	out 92h, al
;__________________________________________________




;__________________________________________________
;es=0
	xor eax, eax
	mov es,ax
	cld

;[0x840,+0x87f]
	mov di,gdtr
	mov cx,0x40
	rep stosb

;[0x3000,0x7fff]
	mov di,0x3000
	mov cx,0x5000
	rep stosb
;_________________________________________________




;__________________________________________________
;在0x810这个位置手工写入jmp rax这条指令
	xor ax,ax
	mov es,ax
	mov di,jmprax
	mov word [es:di],0xe0ff		;e0ff=jmp rax
;___________________________________________________




;__________________________________________________ 
;gdt表位于
	mov di,gdthome

;第一个:必须是空描述符
	xor eax,eax
	stosd
	stosd

;第二个:64bit代码段00,00,00,00,00,9a,2f,00
	stosd
	mov eax,0x002f9a00
	stosd

;第三个:64bit数据段00,00,00,00,00,92,2f,00
	xor eax,eax
	stosd
	mov eax,0x002f9200
	stosd




;gdtr位于
	mov di,gdtr
;gdtr.size=gdt表的总长度-1
	mov ax,0x17
	stosw			;8*3 - 1
;gdtr.addr=gdt表的位置
	mov eax,gdthome
	stosd
	xor eax,eax
	stosd
;__________________________________________________




;___________________________________________________
;关闭中断
	mov al, 0xFF
	out 0xA1, al
	nop
	out 0x21, al
	nop

;一个空的IDTR,任何可屏蔽中断都会三重错误
	mov di,idtr			;now es=0
	xor eax,eax
	stosw				;length
	stosd				;base

;lidt指令
	lidt [idtr]		;load pointer
;___________________________________________________
 



;___________________________________________________
;在pd里面写0|0x83
	mov word [temppd], 0x0|0x83

;把pd的地址放入pdpt
	mov word [temppdpt], temppd|3

;把pdpt的地址放入pml4
	 mov word [temppml4], temppdpt|3
;___________________________________________________




;__________________________________________________
;Set the PAE and PGE bit.
	mov eax,10100000b
	mov cr4, eax
;Point CR3 at the PML4.
	mov edx,temppml4
	mov cr3, edx
;读出msr寄存器,然后把它的long mode enable位设置为1
	mov ecx,0xC0000080        ;Read from the EFER MSR. 
	rdmsr    
	or eax,0x00000100        ; Set the LME bit.
	wrmsr
;___________________________________________________




;______________________打开sse______________________
;读出cr0,改掉,写回去
	mov eax,cr0
	and ax,0xFFFB	;clear coprocessor emulation CR0.EM
	or ax,0x2	;set coprocessor monitoring  CR0.MP
	mov cr0,eax
;读出cr4,改掉,写回去
	mov eax,cr4
	or ax,3<<9	;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
	mov cr4,eax
;_______________________________________________




;___________________正式进入_____________________
;读出cr0,再enable paging and protection simultaneously,然后写回去
	mov ebx, cr0
	or ebx,0x80000001
	mov cr0, ebx                    

;下面一大堆只是为了地址无关,否则lgdt和jmp两条指令就能完事
	xor eax,eax
	xor ebx,ebx

	push cs				;取出段基址给eax
	pop ax
	shl eax,4

	call where
where:
	pop bx
	add eax,ebx			;where的实际地址

	add eax,endofenter64-where	;endofenter64的实际地址

;Load pointer (a32 because over 64k)
	a32 lgdt [gdtr]

;jmp rax: Load CS+flush instruction cache
	jmp dword 0x0008:jmprax
;______________________________________________




paddingofenter64:
times 0x400-(paddingofenter64-startofenter64) db 0

endofenter64:
