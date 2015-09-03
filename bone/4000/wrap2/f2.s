[bits 64]




;____________________________________________
f2init:
	ret
;___________________________________________




;_________________picture_______________________
f2show:
;	mov edi,0x1000000

;	mov rbx,[0x4fd8]
;	cmp rbx,768
;	jb .yok
;	mov rbx,760
;.yok:
;	shl rbx,2+10		;每行4096B=0x1000=2^12
;	add rdi,rbx

;	mov rax,[0x4fc8]		;鼠标
;	cmp rax,1024
;	jb .xok
;	mov rax,1020
;	.xok:
;	shl rax,2
;	add rdi,rax

;	mov rcx,16
;.continue:
;	mov rax,0x123456789abcdef
;	mov [rdi],rax
;	mov [rdi+8],rax
;	mov [rdi+16],rax
;	mov [rdi+24],rax
;	mov [rdi+32],rax
;	mov [rdi+40],rax
;	mov [rdi+48],rax
;	mov [rdi+56],rax
;	add rdi,4096
;	loop .continue

	mov rbp,[rel rgbabase]
	jmp writescreen
;_________________________________________
rgbabase:dq 0x1c00000			;16m+4m








;_________________________________________
f2event:

.left:
	cmp al,0x4b
	jne .notleft

	cmp qword [rel rgbabase],0x400000
	jb .return
	sub qword [rel rgbabase],0x400000
	ret
.notleft:

.right:
	cmp al,0x4d
	jne .notright

	add qword [rel rgbabase],0x400000
	ret
.notright:

.up:
	cmp al,0x48
	jne .notup

	cmp qword [rel rgbabase],0x10000
	jb .return
	sub qword [rel rgbabase],0x10000
	ret
.notup:

.down:
	cmp al,0x50
	jne .notdown

	add qword [rel rgbabase],0x10000
	ret
.notdown:

.return:
	ret
;________________________________________
