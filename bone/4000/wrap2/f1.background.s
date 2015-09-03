[bits 64]




;____________________________________
changeaddress:
	mov rax,[rel input]
	mov rbx,rax
	and rax,0xfffffffffffffc00
	and rbx,0x3ff
	mov [rel addr],rax
	mov [rel offset],rbx
	mov qword [rel input],0
	ret
;___________________________________




;________________________________________________
changesector:
	mov rsi,0x180000
	.continue:
	cmp dword [rsi],"read"
	je .findit
	add rsi,0x10
	cmp rsi,0x180000+0x1000
	jb .continue
	ret

.findit:
	mov rdx,[rsi+8]
	mov rdi,[rel input]
	mov [rel sector],rdi
	and rdi,0xfffffffffffffff8
	call rdx

	mov rax,[rel input]
	and rax,0x7
	shl rax,9
	add rax,0x2000000
	mov [rel addr],rax
	mov qword [rel input],0
	ret
;_________________________________________________




;____________________________________
changeoffset:
	mov rax,[rel input]
	cmp rax,0xbff
	jna .notabove
	ret
.notabove:
	mov [rel offset],rax
	mov qword [rel input],0
	ret
;_____________________________________




;________________________________
changememory:
	mov eax,[rel input]
	mov rdi,[rel addr]
	add rdi,[rel offset]
	mov [rdi],eax
	mov qword [rel input],0
	ret
;_______________________________




;__________________________________
changeview:
	inc byte [rel viewmode]
	test byte [rel viewmode],1
	jnz .anscii
.hex:
	lea rax,[rel dumphex]
	mov [rel hexoranscii],rax
	ret
.anscii:
	lea rax,[rel dumpanscii]
	mov [rel hexoranscii],rax
	ret
.utf8:
	;?
.port:
	;?
;__________________________________
viewmode:dd 1




;______________________________________
searchinthisscreen:
	mov eax,[rel input]
	mov edi,[rel addr]
	mov ebx,0
.loopsearch:
	cmp [edi+ebx],eax
	je .findit
	inc ebx
	cmp ebx,0xc00
	jb .loopsearch
	ret
.findit:
	mov [rel offset],rbx
	ret
;___________________________________




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




;_____________________________________________
f1backgroundevent:




.left:
	cmp al,0x4b
	jne .notleft

.leftcheck:
	mov rax,[rel offset]				;mov bl,0x40;div bl
	and al,0x3f
	cmp al,0
	je .leftedge

.leftnormal:
	dec qword [rel offset]

	;mov rax,al
	;mov dword [rel f1change],0			;只刷新offset对应的屏幕位置
	ret

.leftedge:
    cmp qword [rel addr],0x800
    jb .return							;什么都不用刷新

    sub qword [rel addr],0x800

	;mov dword [rel f1change],0xffff	;全部要刷新
    ret

.notleft:




.right:
	cmp al,0x4d
	jne .notright

	mov rax,[rel offset]
	mov bl,0x40
	div bl
	cmp ah,0x3f
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
	jb .upbelow
	sub qword [rel offset],0x40
	ret
.upbelow:
    cmp qword [rel addr],0x40
    jb .return
    sub qword [rel addr],0x40
    ret
.notup:




.down:
	cmp al,0x50
	jne .notdown

	cmp qword [rel offset],0xbbf
	ja .downabove
	add qword [rel offset],0x40
	ret
.downabove:
    add qword [rel addr],0x40
    ret
.notdown:




.other:
	;在本页里面搜索~




.return:
	ret
;________________________________________________