startofpci:
[BITS 64]
;进:
;出:esi
;用:
;变:
;__________old or new_____________
pcie:
    mov esi,0x4028            ;si不对
.search:
    cmp dword [esi],"MCFG"
    je pcienew
    add si,0x10
    cmp si,0x4400
    jb .search

    death:hlt
    jmp death
;_________________________________


;进:esi
;出:
;用:
;变:
;______________pcie(mcfg)__________________
pcienew:
    mov esi,[esi-0x8]           ;["MCFG"-0x8]=mcfg address
    mov esi,[esi+0x2c]          ;[mcfg+0x2c]=pcie base
    mov edi,0x5000
    mov ecx,0xffff

.newenumeration:
    mov eax,[esi]
    cmp eax,0xffffffff
    je .next

        mov [edi],esi
        add edi,8
        ;stosd
        mov eax,[esi+0x8]           ;class,subclass       
        stosq

.next:
    add esi,0x1000
    cmp edi,0x5fff
    ja endofpci
    loop .newenumeration
;________________________________

    jmp endofpci

paddingofpci:
times 0x400-(paddingofpci-startofpci) db 0

endofpci:
