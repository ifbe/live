[bits 64]




;_____________________________________
dumpanscii:
	mov rax,[rel addr]
	mov [rel ansciipointer],rax
	mov dword [rel ansciicount],0
.ansciiline:
	mov edi,0x1000000			;locate destination
	mov eax,[rel ansciicount]
	imul eax,4*1024*16
	add edi,eax
	call dumpanscii64
	inc byte [rel ansciicount]
	cmp byte [rel ansciicount],0x30
	jb .ansciiline

	ret
;______________________________________
ansciipointer:dq 0
ansciicount:dd 0




;_______________________________
dumpanscii64:
	mov ecx,0x40
.dump:
	mov al,0x20
	call char

	mov rbx,[rel ansciipointer]
	mov al,[rbx]
	cmp al,0x20
	jb .blank
	cmp al,0x80
	jb .visiable

.blank:
	mov al,0x20

.visiable:
	call char
	inc qword [rel ansciipointer]

	dec cl
	jnz .dump

	ret
;_______________________________




;_____________________________________
dumphex:
	mov rax,[rel addr]
	mov [rel hexpointer],rax
	mov dword [rel hexcount],0
.hexline:
	mov edi,0x1000000			;locate destination
	mov eax,[rel hexcount]
	imul eax,4*1024*16
	add edi,eax
	call dumphex64
	inc byte [rel hexcount]
	cmp byte [rel hexcount],0x30
	jb .hexline

	ret
;______________________________________
hexpointer:dq 0
hexcount:dd 0




;_______________________________
dumphex64:
	mov cl,0x10
.dump4:
	mov rax,[rel hexpointer]
	mov eax,[rax]
	mov [rel pcimust32],eax

	mov ch,4
.dump:
	mov al,[rel pcimust32]
	shr al,4
	call character
	mov al,[rel pcimust32]
	and al,0xf
	call character
	shr dword [rel pcimust32],8
	dec ch
	cmp ch,0
	jne .dump

	add qword [rel hexpointer],4
	dec cl
	jnz .dump4

	ret
;_______________________________
pcimust32:dd 0








;_______________________________________
f1showbackground:
	cmp dword [rel offsetold],0xffff
	jb .skip

	call [rel hexoranscii]

    mov rbp,0x1000000				;[16m,20m)
	call writescreen

.skip:
	ret
;_______________________________________
addr:dq 0				;当前页面位置
sector:dq 0				;扇区号