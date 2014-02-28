[bits 64]
startofapic:
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
    and eax,0xfffff000
    mov [localapicaddress],eax    ;save lapic base

    cmp eax,0xfee00000
    jb endofapic
;_____________________________________


;___________________________________
savelapic:
    mov edi,0x4000
.continue:
    cmp dword [edi],0
    jne .addandagain
	mov eax,[localapicaddress]
	stosq
	mov rax,"lapic"
	stosq
	jmp .endofthis
.addandagain:
    add edi,0x10
    cmp edi,0x5000
    jb .continue
;___________________________________
.endofthis:


;________________________________________
localapicinitialization:

    mov edi,[localapicaddress]

    ;[+80]=0              (task priority=0)
    xor eax,eax
    mov [edi+0x80],eax    ;(cr8=0<<4)

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
    mov eax,0x170
    mov [edi+0xf0],eax

;_________________________________________

    jmp endoflocalapic


;___________________________
localapicaddress:dq 0
;_____________________________

endoflocalapic:









;_______________________________________
searchioapic:
    mov edi,0x4008
.search:
    cmp dword [edi],"MADT"
    je findioapic
    cmp dword [edi],"APIC"
    je findioapic
    add edi,0x10
    cmp edi,0x5000
    jb .search
;_______________________________________-

    jmp endofapic


;_________________________________
findioapic:

getinformation:
    mov edi,[edi-8]
    mov [madtstart],edi
    mov eax,[edi+4]
    add eax,edi
    mov [madtend],eax
    add edi,0x2c
    mov [tablestart],edi

analyse:
    cmp byte [edi],1
    jne .continue

	mov eax,[edi+4]
	mov [ioapicaddress],eax
	cmp eax,0xfec00000
	jne endofapic
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
    mov edi,0x4000
.continue:
    cmp dword [edi],0
    jne .addandagain

	mov eax,[ioapicaddress]
	stosq
	mov rax,"ioapic"
	stosq
	jmp .endofthis

.addandagain:
    add edi,0x10
    cmp edi,0x5000
    jb .continue
;___________________________________
.endofthis:


;____________________________________
ioapicinitialization:

disable8259:
    mov al, 0xff
    out 0xa1, al
    out 0x21, al

mov edi,[ioapicaddress]
mov ecx,24
mov eax,0x10

maskall:
    mov [edi],eax
    inc eax
    mov dword [edi],0x10000

    mov [edi],eax
    inc eax
    mov dword [edi+0x10],0

loop maskall
;____________________________________


jmp endofapic


paddingofapic:
times 0x800-(paddingofapic-startofapic) db 0


endofapic:
