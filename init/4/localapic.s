[bits 64]
startoflocalapic:
;______________________________
detectlocalapic:
    mov eax,1
    cpuid
    bt edx,9
    jnc endoflocalapic
;______________________________


;_____________________________________
findlocalapic:
    mov rcx,0x1b
    rdmsr
    ;bts eax,11            ;enable lapic
    ;wrmsr
    and eax,0xfffff000
    mov [rel localapicaddress],eax    ;save lapic base

    cmp eax,0xfee00000
    jb endoflocalapic
;_____________________________________


;___________________________________
savelapic:
    mov edi,0x4000
.continue:
    cmp dword [edi],0
    jne .addandagain
	mov eax,[rel localapicaddress]
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

    mov edi,[rel localapicaddress]

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


localapicaddress:dq 0

paddingoflocalapic:
times 0x200-(paddingoflocalapic-startoflocalapic) db 0

endoflocalapic:
