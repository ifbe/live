%define journalhome 0xd00000
[bits 64]




;_________________清空/journal________________
f1init:
	mov qword [rel addr],journalhome		;r14 memory pointer
	mov qword [rel offset],0x420			;r15 offset pointer

	lea rax,[rel dumpanscii]
	mov [rel printmethod],rax

	ret
;_______________________________________________








;_________________________________________________
f1show:
.background:
	cmp dword [rel offsetold],0xffff
	jb .skipbackground

	call [rel printmethod]

    mov rbp,0x1000000				;[16m,20m)
	call writescreen
.skipbackground:




.foreground:
.clean:
	mov r8,[rel offsetold]
	cmp r8,0xffff
	jae .skipclean

	shr r8,6
	shl r8,4+16						;(y<<16)
	mov r8w,[rel offsetold]
	and r8w,0x3f
	shl r8w,4						;(x)
	call cleanmouse
	call cleanmenu
.skipclean:




.new:
	mov r8,[rel offset]
	shr r8,6
	shl r8,4+16						;(y<<16)
	mov r8w,[rel offset]
	and r8w,0x3f
	shl r8w,4						;(x)
	call mousenew
	call menu




.return:
	ret
;_____________________________________________
printmethod:dq 0

addr:dq 0				;当前页面位置

offsetold:dq 0
offset:dq 0				;“鼠标”位置








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
	mov [rel printmethod],rax
	ret
.anscii:
	lea rax,[rel dumpanscii]
	mov [rel printmethod],rax
	ret
.nottab:




.left:
	cmp al,0x4b
	jne .notleft

.leftcheck:
	mov rax,[rel offset]				;mov bl,0x40;div bl
	and al,0x3f
	cmp al,0
	je .leftedge

.leftnormal:
	mov rax,[rel offset]
	mov [rel offsetold],rax			;只刷新offset对应的屏幕位置

	dec qword [rel offset]
	ret

.leftedge:
    cmp qword [rel addr],0x800
    jb .return							;什么都不用刷新

	mov dword [rel offsetold],0xffff	;全部要刷新

    sub qword [rel addr],0x800
    ret
.notleft:




.right:
	cmp al,0x4d
	jne .notright

.rightcheck:
	mov rax,[rel offset]
	and al,0x3f
	cmp al,0x3f
	je .rightedge

.rightnormal:
	mov rax,[rel offset]
	mov [rel offsetold],rax

	inc qword [rel offset]
	ret

.rightedge:
	mov dword [rel offsetold],0xffff

    add qword [rel addr],0x800
    ret
.notright:




.up:
	cmp al,0x48
	jne .notup

.upcheck:
	cmp qword [rel offset],0x40
	jb .upedge

.upnormal:
	mov rax,[rel offset]
	mov [rel offsetold],rax

	sub qword [rel offset],0x40
	ret

.upedge:
    cmp qword [rel addr],0x40
    jb .return

	mov dword [rel offsetold],0xffff

    sub qword [rel addr],0x40
    ret
.notup:




.down:
	cmp al,0x50
	jne .notdown

.downcheck:
	cmp qword [rel offset],0xbbf
	ja .downedge

.downnormal:
	mov rax,[rel offset]
	mov [rel offsetold],rax

	add qword [rel offset],0x40
	ret

.downedge:
	mov dword [rel offsetold],0xffff

    add qword [rel addr],0x40
    ret
.notdown:




.other:
	jmp f4menumessage




.return:
	ret
;____________________________________________________
tabkey:db 0				;dumpanscii还是dumphex