[bits 64]


startofioapic:
;_______________________________________
searchioapic:
    mov edi,0x4000
.search:
    cmp dword [edi],"MADT"
    je findioapic
    cmp dword [edi],"APIC"
    je findioapic
    add edi,0x10
    cmp edi,0x5000
    jb .search
;_______________________________________-

    jmp endofioapic


;_________________________________
findioapic:

getinformation:
    mov edi,[edi+8]
    mov [rel madtstart],edi
    mov eax,[edi+4]
    add eax,edi
    mov [rel madtend],eax
    add edi,0x2c
    mov [rel tablestart],edi

analyse:
    cmp byte [edi],1
    jne .continue

	mov eax,[edi+4]
	mov [rel ioapicaddress],eax
	cmp eax,0xfec00000
	jne endofioapic
	jmp saveioapic            ;find it,lets go

.continue:
    mov al,[edi+1]
    movzx eax,al
    add edi,eax
    cmp edi,[rel madtend]
    jb analyse
;_____________________________

    jmp endofioapic             ;cannot find


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

	mov rax,"ioapic"
	stosq
	mov eax,[rel ioapicaddress]
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

mov edi,[rel ioapicaddress]
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


jmp endofioapic


paddingofioapic:
times 0x200-(paddingofioapic-startofioapic) db 0


endofioapic:
