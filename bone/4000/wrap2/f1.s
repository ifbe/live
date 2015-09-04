%define journalhome 0xd00000
[bits 64]




;_________________清空/journal________________
f1init:
	mov qword [rel addr],journalhome		;r14 memory pointer
	mov qword [rel offset],0x420			;r15 offset pointer

	lea rax,[rel dumpanscii]
	mov [rel hexoranscii],rax

	ret
;_______________________________________________








;_________________________________________________
f1show:
	call f1showbackground
	call f1showforeground

	ret
;_____________________________________________
hexoranscii:dq 0



 




;_________________________________________________
f1event:
	cmp al,0x80
	jae .return




.tab:
	cmp al,0xf
	jne .nottab

	mov dword [rel offsetold],0xffff

	inc byte [rel tabkey]
	test byte [rel tabkey],1
	jz .anscii
.hex:
	lea rax,[rel dumphex]
	mov [rel hexoranscii],rax
	ret
.anscii:
	lea rax,[rel dumpanscii]
	mov [rel hexoranscii],rax
	ret
.nottab:




.esc:
	cmp al,0x01
	jne .notesc

	inc byte [rel esckey]
	ret
.notesc:




.other:
	test byte [rel esckey],1
	jnz f1backgroundevent
	jmp f1foregroundevent




.return:
	ret
;____________________________________________________

esckey:db 0				;显不显示menu
tabkey:db 0				;dumpanscii还是dumphex