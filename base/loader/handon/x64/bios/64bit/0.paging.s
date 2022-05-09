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




;_______________clear [80000,a0000]______________
	mov rdi,0x80000
	xor rax,rax
	mov ecx,0x4000
	cld
	stosq
;_______________________________________________




;________pml1(pt), 分太细太耗地方没用它___________
;______________________________________________




;________pml2(pd), 每项2M * 每表512项 * 16个表________
	mov rdi,0x80000
	mov rax,0x87
pagedirectory:
	stosq
	add rax,0x200000
	cmp rdi,0x90000
	jb pagedirectory
;________消耗0x10000B, 代表16GB______________________




;________pml3(pdp), 每项是个pd表地址 * 每表512项目________
	mov edi,0x91000
	mov eax,0x80007
	mov ecx,16
.pdpt:
	stosq
	add eax,0x1000
	loop .pdpt
;________上面填了16张表, 所以这儿只填16项________________




;________pml4, 每项是个pdp表地址 * 每表512项________
;把pdpt的地址放入pml4
	mov dword [0x90000],0x91007
;_______上面填了1张表, 所以这儿只填1项________________




;________cr3, 里面填pml4地址__________________________
	mov rax,0x90000
	mov cr3,rax

	jmp endofpaging
;_____________________________________________________




paddingofpaging:
times 0x400-(paddingofpaging-startofpaging) db 0

endofpaging:
