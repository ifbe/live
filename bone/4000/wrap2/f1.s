%define journalhome 0xd00000
[bits 64]




;_________________清空/journal________________
f1init:
	mov qword [rel addr],journalhome		;r14 memory pointer
	mov qword [rel offset],0x420			;r15 offset pointer

	lea rax,[rel menu]
	mov [rel mouseormenu],rax

	lea rax,[rel dumpanscii]
	mov [rel hexoranscii],rax

	ret
;_______________________________________________








;_________________________________________________
f1show:
	call [rel hexoranscii]
	call [rel mouseormenu]

    mov rbp,0x1000000				;[16m,20m)
	jmp writescreen
;_____________________________________________
mouseormenu:dq 0
hexoranscii:dq 0








;_________________________________________________
f1event:
	cmp al,0x80
	jae .return




.esc:
	cmp al,0x01
	jne .notesc
	inc byte [rel esckey]
	test byte [rel esckey],1
	jnz .escanscii
.eschex:
    lea rax,[rel menu]
    mov [rel mouseormenu],rax
    ret
.escanscii:
    lea rax,[rel mouse]
    mov [rel mouseormenu],rax
    ret
.notesc:




.tab:
	cmp al,0xf
	jne .nottab
	cmp byte [rel chosen],6				;;可以优化成(x+1)&0x7
	ja .tababove
	inc byte [rel chosen]
	ret
.tababove:
	mov byte [rel chosen],0
	ret
.nottab:




.backspace:
	cmp al,0xe
	jne .notbackspace
	shr qword [rel input],4
	ret
.notbackspace:




.left:
	cmp al,0x4b
	jne .notleft
	mov rax,[rel offset]
	mov bl,0x40
	div bl
	cmp ah,0
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




.enter:
	cmp al,0x1c
	jne .notenter
	test byte [rel esckey],1
	jnz .return
	cmp byte [rel chosen],0
	je changeaddress
	cmp byte [rel chosen],1
	je changesector
	cmp byte [rel chosen],2
	je changeoffset
	cmp byte [rel chosen],4
	je searchinthisscreen
	cmp byte [rel chosen],5
	je changememory
	cmp byte [rel chosen],6
	je changeview
	cmp byte [rel chosen],7
	je poweroff
	ret
.notenter:




.other:
	call scan2hex

	cmp al,0xf		;<0x30
	jna .notabove
	ret
.notabove:
	shl qword [rel input],4
	add [rel input],al

.return:
	ret
;____________________________________________________
esckey db 0