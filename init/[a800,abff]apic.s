[bits 64]
startofapic:

localapicinitialization:
;______________________________
detectlocalapic:
    mov eax,1
    cpuid
    bt edx,9
    jnc endofapic
;______________________________


;_____________________________________
findlocalapic:
    mov rcx,0x1b
    rdmsr
    ;bts eax,11            ;enable lapic
    ;wrmsr
    and eax,0xfffff000    ;save lapic base
    mov [localapicaddress],eax
;_____________________________________


;___________________________________
savelapic:
    mov edi,0x8000
.continue:
    cmp dword [edi],0
    je putlapic
    add edi,0x10
    cmp edi,0x9000
    jb .continue

    jmp disable8259
;___________________________________


;____________________________
putlapic:
    mov eax,[localapicaddress]
    stosq
    mov rax,"lapic"
    stosq
;___________________________


;_______________________________________
disable8259:
    mov al, 0xff
    out 0xa1, al
    out 0x21, al
;_________________________________________


;_____________________________________
disablecache1:
;_______________________________________


;________________________________________
    mov edi,[localapicaddress]

    ;[+80]=0              (task priority=0)
    xor eax,eax
    mov [edi+0x80],eax    ;(cr3=0<<4)

    ;[+320]=10000         (disable timer interrupt)
    mov eax,0x10000
    mov [edi+0x320],eax

    ;[+340]=10000         (disable performance counter interrupt)
    mov eax,0x10000
    mov [edi+0x340],eax

    ;[+350]=8700          (disable local interrupt 0(normal external interrupt))
    mov eax,0x8700
    mov [edi+0x350],eax

    ;[+360]=400           (disable local interrupt 1(normal nmi processing))
    mov eax,0x400
    mov [edi+0x360],eax

    ;[+370]=10000         (disable error interrupt)
    mov eax,0x10000
    mov [edi+0x370],eax

    ;[+f0]=10f            (disable spurious vector+enable apic)
    mov eax,0x10f
    mov [edi+0xf0],eax

;_________________________________________

    jmp ioapicinitialization


;___________________________
localapicaddress:dq 0
;_____________________________




















ioapicinitialization:

;_______________________________________
searchioapic:
    mov edi,0x8008
    search:
    cmp dword [edi],"MADT"
    je findioapic
    cmp dword [edi],"APIC"
    je findioapic
    add edi,0x10
    cmp edi,0x9000
    jb search
;_______________________________________-

    jmp endofapic


;_________________________________
findioapic:
    mov edi,[edi-8]
    mov [madtstart],edi
    mov eax,[edi+4]
    add eax,edi
    mov [madtend],eax
    add edi,0x2c
    mov [tablestart],edi
;_______________________________________


;______________________________
analyse:
    cmp byte [edi],1
    jne .continue

    mov eax,[edi+4]
    mov [ioapicaddress],eax
    jmp saveioapic            ;find it,lets go

.continue:
    mov al,[edi+1]
    movzx eax,al
    add edi,eax
    cmp edi,[madtend]
    jb analyse
;_____________________________

    jmp endofapic             ;cannot find

;________________________________________
madtstart:dq 0
madtend:dq 0
tablestart:dq 0
ioapicaddress:dq 0
;___________________________________


;___________________________________
saveioapic:
    mov edi,0x8000
.continue:
    cmp dword [edi],0
    je putioapic
    add edi,0x10
    cmp edi,0x9000
    jb .continue
;___________________________________
    jmp ioapicinit


;____________________________
putioapic:
    mov eax,[ioapicaddress]
    stosq
    mov rax,"ioapic"
    stosq
;___________________________


;____________________________________
ioapicinit:
;____________________________________

paddingofapic:
times 0x400-(paddingofapic-startofapic) db 0


endofapic:
