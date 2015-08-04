[BITS 64]
startofprepare64:


;_____________environment___________________
	mov ax,0x0010
	mov ds,ax
	mov es,ax
	mov fs,ax
	mov gs,ax
	mov ss,ax
	mov rsp,0xa0000          ;..........16MB..........be careful
;__________________________________




;_______________clear [80000,98000]______________
	mov rdi,0x80000
	xor rax,rax
	mov ecx,0x3000
	cld
	stosq
;_______________________________________________




;________________0x2000个pd____________
	mov rdi,0x80000
	mov rax,0x83
pagedirectory:
	stosq
	add rax,0x200000
	cmp rdi,0x90000
	jb pagedirectory
;________________________________________




;________________16个pdpt_________________
	mov edi,0x91000
	mov eax,0x80003
	mov ecx,16
.pdpt:
	stosq
	add eax,0x1000
	loop .pdpt
;______________________________________




;________________________________________________
;把pdpt的地址放入pml4
	mov dword [0x90000],0x91003

;把pml4的地址放进cr3
	mov rax,0x90000
	mov cr3,rax

;完事,跳到下一块继续执行
	jmp endofprepare64
;________________________________________




paddingofprepare64:
times 0x100-(paddingofprepare64-startofprepare64) db 0

endofprepare64:
