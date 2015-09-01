[bits 64]




;>>>>>>>>>>>>>>>>>>>>>>>>-----F5----<<<<<<<<<<<<<<<<<<<<<<<<<<<
;______________________________________________
f5show:
	mov rsi,[rel addr]
	mov rdi,0xb8000
	mov ah,0x7		;color
.lines:
	mov rcx,0x40
.oneline:
	lodsb
	stosw
	loop .oneline
	mov rcx,0x10
	mov al,0x20
	rep stosw

	cmp rdi,0xb8f00
	jb .lines

	mov rbx,[rel addr]
	call putrbx
	mov rbx,[rel offset]
	call putrbx

	mov rax,[rel offset]
	mov bl,0x40
	div bl			;商是al，余数是ah
	mov bl,ah
	movzx rbx,bl		;余数
	movzx rax,al		;商
	imul rax,0xa0
	shl rbx,1
	mov rdi,0xb8000
	add rdi,rax
	add rdi,rbx
	mov byte [rdi+1],0xf0

	ret
;_______________________________________________




;________________________________________________
f5event:
.left:
	cmp al,0x4b
	jne .notleft

	mov rax,[rel offset]
	and ax,0x3f
	cmp ax,0
	je .leftequal
	dec qword [rel offset]
	ret
.leftequal:
	cmp qword [rel addr],0x800
	jb .return
	sub qword [rel addr],0x800
	ret
.notleft:

.right:
	cmp al,0x4d
	jne .notright

	mov rax,[rel offset]
	je .rightequal
	inc qword [rel offset]
	ret
.rightequal:
	add qword [rel addr],0x800
	ret
.notright:

.up:
	cmp al,0x48
	jne .notup

	cmp qword [rel offset],0x40
	jb .below1
	sub qword [rel offset],0x40
	ret
.below1:
	cmp qword [rel addr],0x40
	jb .return
	sub qword [rel addr],0x40
	ret
.notup:

.down:
	cmp al,0x50
	jne .notdown

	cmp qword [rel offset],0x5bf
	ja .above1
	add qword [rel offset],0x40
	ret
.above1:
	add qword [rel addr],0x40
	ret
.notdown:

.return:
	ret
;______________________________________________








;________________________________________________
putrbx:
	mov rcx,16
	mov ah,0xf
	mov byte [rel .nonzero],0
.continue:
	rol rbx,4
	mov al,bl
	and al,0xf

	cmp byte [rel .nonzero],1
	je .normal
	cmp al,0
	jne .notnormal
	mov al,0x20
	jmp .skip
	.notnormal:
	mov byte [rel .nonzero],1
.normal:
	add al,0x30
	cmp al,0x3a
	jb .skip
	add al,0x7
.skip:
	stosw
	loop .continue

.return:
	ret
;________________________________________
.nonzero:db 0
