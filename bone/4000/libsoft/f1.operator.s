[bits 64]




;____________________________________
changeaddress:
	lea r8,[rel arg1]
	call string2data

	mov rax,[rel value]
	mov rbx,rax
	and rax,0xfffffffffffffc00
	and rbx,0x3ff
	mov [rel addr],rax
	mov [rel offset],rbx
	ret
;___________________________________




;________________________________
changememory:
	lea r8,[rel arg1]
	call string2data

	mov eax,[rel value]
	mov rdi,[rel addr]
	add rdi,[rel offset]
	mov [rdi],eax
	ret
;_______________________________




;______________________________________
searchinthisscreen:
	lea r8,[rel arg1]
	call string2data

	mov al,[rel value]
	mov edi,[rel addr]
	xor ecx,ecx

.continue:
	cmp [edi+ecx],al
	je .findit

	inc ecx
	cmp ecx,0xc00
	jb .continue
	ret

.findit:
	mov [rel offset],rcx
	ret
;___________________________________



















;________________________________________________
;changesector:
;	mov rsi,0x180000
;	.continue:
;	cmp dword [rsi],"read"
;	je .findit
;	add rsi,0x10
;	cmp rsi,0x180000+0x1000
;	jb .continue
;	ret

;.findit:
;	mov rdx,[rsi+8]
;	mov rdi,[rel input]
;	mov [rel sector],rdi
;	and rdi,0xfffffffffffffff8
;	call rdx

;	mov rax,[rel input]
;	and rax,0x7
;	shl rax,9
;	add rax,0x2000000
;	mov [rel addr],rax
;	mov qword [rel input],0
;	ret
;_________________________________________________




;____________________________________
;changeoffset:
;	mov rax,[rel input]
;	cmp rax,0xbff
;	jna .notabove
;	ret
;.notabove:
;	mov [rel offset],rax
;	mov qword [rel input],0
;	ret
;_____________________________________




;__________________________________
;changeview:
;	inc byte [rel viewmode]
;	test byte [rel viewmode],1
;	jnz .anscii
;.hex:
;	lea rax,[rel dumphex]
;	mov [rel hexoranscii],rax
;	ret
;.anscii:
;	lea rax,[rel dumpanscii]
;	mov [rel hexoranscii],rax
;	ret
;.utf8:
;	;?
;.port:
;	;?
;__________________________________
;viewmode:dd 1