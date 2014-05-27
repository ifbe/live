[BITS 64]


;________________________________
startofprepare64:
;environment
    mov ax,0x0010
    mov ds,ax
    mov es,ax
    mov fs,ax
    mov gs,ax
    mov ss,ax
    mov rsp,0xa0000          ;..........16MB..........be careful
;__________________________________




;____________________________________
    mov rdi,0x80000
    xor rax,rax
    mov ecx,0x3000
    cld
    stosq				;clear [80000,98000]
;_____________________________________




;_______________________________________
;16 page directorys
    mov rdi,0x80000
    mov rax,0x83
pagedirectory:
    stosq
    add rax,0x200000
    cmp rdi,0x90000
    jb pagedirectory
;________________________________________




;_________________________________
    mov edi,0x91000			;[pdpt]=pd
    mov eax,0x80003
    mov ecx,16
.pdpt:
    stosq
    add eax,0x1000
    loop .pdpt

    mov dword [0x90000],0x91003		;[pml4]=pdpt
;______________________________________




;___________________________________
    mov rax,0x90000
    mov cr3,rax
;_____________________________________
    jmp endofprepare64




paddingofprepare64:
times 0x200-(paddingofprepare64-startofprepare64) db 0

endofprepare64:
