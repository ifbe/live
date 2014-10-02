startofacpi:
[BITS 64]
;进:
;出:rbp
;用:
;变:rax,rbp,cx,*0x4008=RSD PTR 
;_________look for rsd ptr from e0000~ffff0__________
    mov rax,"RSD PTR "
    mov edi,0x4000
    stosq
    mov rbp,0xe0000
    mov cx,0x2000
rsdptr:
    cmp [rbp],rax
    je findrsdptr
    add rbp,0x10
    loop rsdptr

    qusiba: hlt             ;if not find
    jmp qusiba
;_____________________________________________


;进:rbp
;出:
;用:
;变:*0x4000=&rsdp
;____________________________________
findrsdptr:
    mov rax,rbp       ;rsdptr
    stosq
    cmp byte [rbp+0xf],0x00 ;不是0就是xsdt
    jne xsdt


;进:rbp
;出:
;用:
;变:eax,rbp,cx,*0x4018="RSDT",*0x4010=&rsdt
;_______________________________
rsdt:
    mov eax,"RSDT"
    stosq
    mov eax,[rbp+0x10]
    stosq
    mov cl,[eax+4]
    sub cl,0x24
    add eax,0x24
    mov edi,0x4020
.rsdttable:
    mov esi,[eax]
    mov [edi+8],esi
    movsd
    add edi,0xc
    add eax,0x4
    sub cl,4
    cmp cl,0
    ja .rsdttable

    jmp acpi
;__________________________________


;进:rbp
;出:
;用:
;变:rax,rbp,cx,*0x4018="RSDT",*0x4010=&rsdt
;____________________________________
xsdt:
    mov eax,"XSDT"
    stosq
    mov rax,[rbp+0x18]
    stosq
    mov cl,[rax+4]
    sub cl,0x24
    add eax,0x24
    mov edi,0x4020
.xsdttable:
    mov esi,[eax]
    mov [edi+0x8],esi
    movsq
    add edi,0x8
    add eax,0x8
    sub cl,8
    cmp cl,0
    ja .xsdttable
;_____________________________________________


;进:
;出:
;用:
;变:rax,edi,ecx，*0xb8000~*0xbffff
acpi:
;_______________search in facp for port________________________________
    mov esi,0x4020            ;8008和8018分别是rsdp和xsdt
.search:
    cmp dword [esi],"FACP"
    je facp
    add si,0x10
    cmp esi,0x4400
    jb .search
facp:
    mov esi,[esi+0x8]           ;[esi-0x8]=facp
    mov ax,[esi+0x40]           ;[facp+0x40]=port
    mov [0x4fc],ax
    mov eax,[esi+0x28]
    mov [edi+8],eax
    mov eax,[eax]
    mov [edi],eax

    mov esi,[edi+0x8]
    mov eax,"_S5_"
dsdt:
    cmp [esi],eax
    je find_S5_
    inc esi
    jmp dsdt
;_________________________________________




find_S5_:
;_________________________________
    xor ah,ah
    mov al,[esi+8]           ;esi=_S5_
    mov cl,10
    shl ax,cl
    mov [0x4fe],ax

    ;below is how to shutdown
    ;mov dx,[0x4fc]    mov ax,[0x4fe]    or ax,0x2000    out dx,ax
;_________________________________________

    jmp endofacpi

paddingofacpi:
times 0x100-(paddingofacpi-startofacpi) db 0


endofacpi:
