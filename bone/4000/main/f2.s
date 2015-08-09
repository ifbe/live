[bits 64]




;_________________picture_______________________
f2show:
	mov edi,0x100000

	mov rbx,[0x4fd8]
	cmp rbx,768
	jb .yok
	mov rbx,760
	.yok:
	shl rbx,2+10		;每行4096B=0x1000=2^12
	add rdi,rbx

	mov rax,[0x4fc8]		;鼠标
	cmp rax,1024
	jb .xok
	mov rax,1020
	.xok:
	shl rax,2
	add rdi,rax

	mov rcx,16
	.continue:
	mov rax,0x123456789abcdef
	mov [rdi],rax
	mov [rdi+8],rax
	mov [rdi+16],rax
	mov [rdi+24],rax
	mov [rdi+32],rax
	mov [rdi+40],rax
	mov [rdi+48],rax
	mov [rdi+56],rax
	add rdi,4096
	loop .continue

	jmp writescreen2
;_________________________________________








;_________________________________________
f2event:
.up:
	cmp al,0x48
	jne .notup

	sub qword [rel jpegbase],0x10000
	ret
.notup:

.down:
	cmp al,0x50
	jne .notdown

	add qword [rel jpegbase],0x10000
	ret
.notdown:

.other:
	ret
;________________________________________