%define pcihome 0x40000
[BITS 64]


startofpci:
;__________old or new_____________
pcie:
    mov esi,0x4020
.search:
    cmp dword [esi],"MCFG"
    je pcienew
    add si,0x10
    cmp si,0x4400
    jb .search

    death:hlt
    jmp death
;_________________________________




;______________pcie(mcfg)__________________
pcienew:
    mov esi,[esi+0x8]           ;["MCFG"-0x8]=mcfg address
    mov esi,[esi+0x2c]          ;[mcfg+0x2c]=pcie base
    mov edi,pcihome+0x10000
    mov ecx,0xffff

.newenumeration:
    mov eax,[esi]
    cmp eax,0xffffffff
    je .next

	stosd
        mov eax,[esi+0x8]           ;class,subclass       
        stosd
        mov [edi],esi
        add edi,8

.next:
    add esi,0x1000
    cmp edi,pcihome+0x10000
    ja endofpci
    loop .newenumeration
;________________________________

    jmp endofpci

paddingofpci:
times 0x100-(paddingofpci-startofpci) db 0

endofpci:
