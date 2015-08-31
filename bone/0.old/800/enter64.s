%define bornhome 0x800
%define jmprax 0x810

%define gdtr 0x840
%define idtr 0x850
%define gdthome 0x4000
%define idthome 0x5000

%define temppd 0x6000
%define temppdpt 0x7000
%define temppml4 0x3000

[BITS 16]
startofenter64:




;___________打开A20地址线______________
	in al, 92h  
	or al, 2  
	out 92h, al
;_________________________________




;__________________________________________
;注意不能清空:e820信息[800,fff],屏幕信息[1000,1fff],栈[7000,7c00]
	xor eax, eax
	mov es,ax
	cld

;[+0x10,+0x1f]:jump rax
;[+0x40,+0x4f]:gdtr
;[+0x50,+0x5f]:idtr
	mov di,gdtr
	mov cx,0x40
	rep stosb

;[0x4000,0x4fff]:gdt
;[0x5000,0x5fff]:idt
	mov di,0x3000
	mov cx,0x5000
	rep stosb
;_________________________________________________




;_______在0x810这个位置手工写入jmp rax这条指令_______
	xor ax,ax
	mov es,ax
	mov di,jmprax
	mov word [es:di],0xe0ff		;e0ff=jmp rax
;___________________________________________________




;_______________global descriptor table_____________ 
;gdt表位于
	mov di,gdthome

;第一个:必须是空描述符
	xor eax,eax
	stosd	;null(lower half)
	stosd	;null(higher half)

;第二个:64bit代码段        0x002f9a00,00000000
	stosd	;code(lower half)
	mov eax,0x002f9a00
	stosd	;code(higher half)

;第三个:64bit数据段        0x002f9200,00000000
	xor eax,eax
	stosd	;data(lower half)
	mov eax,0x002f9200
	stosd	;data(higher half)




;gdtr位于
	mov di,gdtr
;gdtr.size=gdt表的总长度-1
	mov ax,0x17
	stosw					;dw $-GDT-1	16bit Size
;gdtr.addr=gdt表的位置
	mov eax,gdthome
	stosd					;low 32bit
	xor eax,eax
	stosd					;high 32bit
;______________________________________________




;_______________________________________
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
;_______________________________________________
 



;__________映射2MB的地址空间足够pre kernel用了________
;在pd里面写0|0x83
	mov word [temppd],0x83					;temppd=0x6000

;把pd的地址放入pdpt(0x91000)(9000:1000)
	mov word [temppdpt],temppd | 3			;temppd=0x6000,temppdpt=0x7000

;把pdpt的地址放入pml4(0x90000)(9000:0000)
	 mov word [temppml4],temppdpt | 3				;temppdpt=0x7000,temppml4=0x3000
;______________________________________________________




;_______开pae/pge,给cr3写地址,设置efer.lme=1_________
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
;____________________________________________




;______________________打开sse______________________
;读出cr0,改掉,写回去
    mov eax,cr0
    and ax,0xFFFB      ;clear coprocessor emulation CR0.EM
    or ax,0x2          ;set coprocessor monitoring  CR0.MP
    mov cr0,eax
;读出cr4,改掉,写回去
    mov eax,cr4
    or ax,3<<9    ;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
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

;目前仍在实模式,取出段基址给eax
	push cs
	pop ax
	shl eax,4

;基地址加上段内偏移得到where的实际地址
	call where    ;往栈里压入where的地址
where:
	pop bx
	add eax,ebx

;再加上相对长度,得到rax=endofenter64的实际地址
	add eax,endofenter64-where

;Load pointer (a32 because over 64k)
	a32 lgdt [gdtr]

;Load CS,flush instruction cache
;内存800位置里面的东西是jmp rax,也就是跳回来到endofenter64的位置
	jmp dword 0x0008:jmprax
;______________________________________________




paddingofenter64:
times 0x400-(paddingofenter64-startofenter64) db 0

endofenter64:
