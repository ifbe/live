[BITS 16]


startofenter64:
;___________A20地址线______________
    in al, 92h  
    or al, 2  
    out 92h, al
;_________________________________


;__________________________________________
    xor ax,ax
    mov es,ax

    ;清空[0x600,0xfff]
    mov di,0x600
    mov cx,0xa00
    xor eax, eax
    cld
    rep stosb
    ;清空[0x4000,0x6fff]
    mov di,0x4000
    mov cx,0x4000
    xor eax, eax
    cld
    rep stosb
;_________________________________________________


;______________________________________________
    mov word [0x6000],0x83

;&pd放入pdpt(0x91000)(9000:1000)
    mov word [0x5000],0x6003

;&pdpt放入pml4(0x90000)(9000:0000)
    mov word [0x4000],0x5003
;_______________________________________




;________________________________
;0x800:jmp rax
xor ax,ax
mov es,ax
mov di,0x800
mov word [es:di],0xe0ff		;e0ff=jmp rax
;__________________________________

 
;____________________关闭中断___________________
    mov al, 0xFF
    out 0xA1, al
    nop
    out 0x21, al
    nop

    mov di,0x600	;now es=0
    xor eax,eax
    stosw		;length
    stosd		;base

    lidt [0x600]	;load pointer空IDT,任何可屏蔽中断都会三重错误
;_______________________________________________
 

;_____________________________________
    mov eax, 10100000b                ; Set the PAE and PGE bit.
    mov cr4, eax
    mov edx,0x4000                      ; Point CR3 at the PML4.
    mov cr3, edx
    mov ecx, 0xC0000080               ; Read from the EFER MSR. 
    rdmsr    
    or eax, 0x00000100                ; Set the LME bit.
    wrmsr
;____________________________________________


;______________________打开sse______________________
    mov eax, cr0
    and ax, 0xFFFB      ;clear coprocessor emulation CR0.EM
    or ax, 0x2          ;set coprocessor monitoring  CR0.MP
    mov cr0, eax
    mov eax, cr4
    or ax, 3 << 9       ;set CR4.OSFXSR and CR4.OSXMMEXCPT at the same time
    mov cr4, eax
;_______________________________________________


;___________________正式进入_____________________
    mov ebx, cr0            ;Activate long mode -
    or ebx,0x80000001       ;by enabling paging and protection simultaneously.
    mov cr0, ebx                    


    mov di,0x700
    xor eax,eax
    stosd	;null(lower half)
    stosd	;null(higher half)	0x0000000000000000	必须的空描述符
    stosd	;code(lower half)
    mov eax,0x002f9a00
    stosd	;code(higher half)	0x002f9a0000000000	64bit 代码段 
    xor eax,eax
    stosd	;data(lower half)
    mov eax,0x002f9200
    stosd	;data(higher half)	0x002f920000000000	64bit 数据段

    mov di,0x7f0
    mov ax,0x17
    stosw	;dw $-GDT-1	16bit Size
    mov eax,0x700
    stosd	;low 32bit
    xor eax,eax
    stosd	;high 32bit

    xor eax,eax			;这么一大堆
    xor ebx,ebx			;
    push cs			;
    pop ax			;
    shl eax,4			;
    call where			;就为了地址无关;
where:				;
    pop bx			;
    add eax,ebx			;
    add eax,endofenter64-where	;rax=endofenter64的地址

    a32 lgdt [0x7f0]  		;Load pointer (a32 because over 64k)
    jmp dword 0x0008:0x0800	;Load CS,flush instruction cache
				;0x0800：jmp rax
;______________________________________________


paddingofenter64:
times 0x100-(paddingofenter64-startofenter64) db 0


endofenter64:
