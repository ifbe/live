[BITS 64]


startofpci:
;___________pcie(0xcf8/0xcfc)______________
pcieold:
    mov edi,0x5000
    mov ebx,0x80000000
    mov ecx,0xffff
.enumeration:
    mov eax,ebx
    mov dx,0xcf8
    out dx,eax
    mov dx,0xcfc
    in eax,dx
    cmp eax,0xffffffff
    je .empty

	mov eax,ebx
        mov dx,0xcf8       ;id
        out dx,eax
        mov dx,0xcfc
        in eax,dx
	mov [edi],eax

        lea eax,[ebx+8]    ;class
        mov dx,0xcf8
        out dx,eax
        mov dx,0xcfc
        in eax,dx
	mov [edi+4],eax

	mov [edi+8],ebx
	add edi,0x10
.empty:
    add ebx,0x100
    loop .enumeration
;________________________________________

    jmp endofpci

paddingofpci:
times 0x200-(paddingofpci-startofpci) db 0

endofpci: