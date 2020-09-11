[BITS 64]
startofpaging:


;_____________environment___________________
	mov eax,0x0010
	;mov ss,ax
	;mov ds,ax
	;mov es,ax
	mov fs,ax
	mov gs,ax
	mov rsp,0xa0000          ;.........@640K........be careful
;__________________________________




;_______________clear [80000,98000]______________
	mov rdi,0x80000
	xor rax,rax
	mov ecx,0x4000
	cld
	stosq
;_______________________________________________




;________________0x2000个pd____________
	mov rdi,0x80000
	mov rax,0x87
pagedirectory:
	stosq
	add rax,0x200000
	cmp rdi,0x90000
	jb pagedirectory
;________________________________________




;________________16个pdpt_________________
	mov edi,0x91000
	mov eax,0x80007
	mov ecx,16
.pdpt:
	stosq
	add eax,0x1000
	loop .pdpt
;______________________________________




;________________________________________________
;把pdpt的地址放入pml4
	mov dword [0x90000],0x91007

;把pml4的地址放进cr3
	mov rax,0x90000
	mov cr3,rax

;完事,跳到下一块继续执行
	jmp endofpaging
;________________________________________




paddingofpaging:
times 0x400-(paddingofpaging-startofpaging) db 0

endofpaging:
