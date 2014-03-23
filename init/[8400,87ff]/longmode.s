;enter long mode directly
startoflongmode:

BITS 16
;___________A20地址线______________
    in al, 92h  
    or al, 2  
    out 92h, al
;_________________________________


;______________________________________
    mov si,no64
    call CheckCPU    ; Check whether we support Long Mode or not.
    jc fail   ; Point edi to a free space bracket.
    jmp SwitchToLongMode
;_________________________________


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
    and eax, 1     ; Check whether bit 21 is set or not. If EAX now contains 0,CPUID isn't supported.
    push ecx
    popfd 
 
    test eax, eax
    jz .no64
 
    mov eax, 0x80000000   
    cpuid                 
 
    cmp eax, 0x80000001               ; Check whether extended function 0x80000001 is available are not.
    jb .no64                ; If not, long mode not supported.
 
    mov eax, 0x80000001  
    cpuid                 
    test edx, 1 << 29                 ; Test if the LM-bit, is set or not.
    jz .no64                ; If not Long mode not supported.
    ret
 
.no64:
    mov si,no64
    stc
    ret
;_________________________________


;________________________________
fail:
    pushad
.PrintLoop:
    lodsb                ; Load the value at [@es:@si] in @al.
    test al, al          ; If AL is the terminator character, stop printing.
    je .PrintDone                  	
    mov ah, 0x0E	
    int 0x10
    jmp .PrintLoop       ; Loop till the null character not found.
 
.PrintDone:
    popad                ; Pop all general purpose registers to save them.
die:hlt
    jmp die
;_____________________________


SwitchToLongMode:
    mov ax,0x9000
    mov ds,ax
    mov es,ax


;__________________________________________
    ;清空0x1000~0x6fff
    mov di,0
    mov cx,0x1800
    xor eax, eax
    cld
    rep stosd
;____________________________________________


;___________&pdpt放入pml4(0x90000)(9000:0000)__________________ 
    mov di,0
    mov eax,0x91003     ;存在&&可写
    mov [es:di],eax     ;放入pml4
;_______________________________________


;_______________&pd放入pdpt(0x91000)(9000:1000)________________
    mov di,0x1000
    mov eax,0x92003
    mov cx,4           ;共4GB
.pdpt:
    mov [es:di],eax
    add eax,0x1000
    add di,8
    loop .pdpt
;_____________________________________


;____________真实地址放入pd([92000,95fff])______________
    mov di,0x2000
    mov eax,0x83

pagedirectory:
    mov [es:di],eax
    add eax,0x200000
    add di,8
    cmp eax,0xffe00000
    jb pagedirectory
    mov [es:di],eax        ;the last one
;________________________________________


    xor eax,eax
    mov ds,ax
    mov es,ax 

 
;____________________关闭中断___________________
    mov al, 0xFF
    out 0xA1, al
    nop
    out 0x21, al
    nop
    lidt [IDT]      ;空IDT,任何可屏蔽中断都会三重错误
    jmp preparation
;_______________________________________________


;____________interrupt descriptor table______________
ALIGN 16
IDT:
    .Length       dw 0
    .Base         dd 0
;__________________________________________________
 

;_________________准备工作____________________
preparation:
    mov eax, 10100000b                ; Set the PAE and PGE bit.
    mov cr4, eax
    mov edx,0x90000                      ; Point CR3 at the PML4.
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
    a32 lgdt [GDT.Pointer]  ;Load GDT.Pointer (a32 because over 64k)
 
    jmp dword 0x0008:alldone;Load CS with 64bit segment&flush instruction cache
;______________________________________________


;_________Global Descriptor Table__________________
GDT:
.Null:    dq 0x0000000000000000    ;必须的空描述符
.code:    dq 0x002f9a0000000000    ;64bit 代码段 
.data:    dq 0x002f920000000000    ;64bit 数据段

ALIGN 4
    dw 0          ;使GDT对其4byte 
.Pointer:
    dw $-GDT-1    ;16-bit Size (Limit) of GDT.
    dq GDT        ;32-bit Base Address of GDT.(CPU will zero extend to 64-bit)
;______________________________________________


;_______________失败字符____________________
no64: db "no64",0
gpu: db "gpu",0
;____________________________________

[BITS 64]
alldone:
    mov ax,0x0010
    mov ss,ax
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov rsp,0x1000000          ;..........16MB..........be careful
    jmp endoflongmode

paddingoflongmode:
times 0x400-(paddingoflongmode-startoflongmode) db 0

endoflongmode:
