[bits 64]




;_______________________________________
f1showforeground:

.cleanmouse:
	mov r8,[rel offsetold]
	cmp r8,0xffff
	jae .skipclean

	shr r8,6
	shl r8,4+16						;(y<<16)
	mov r8w,[rel offsetold]
	and r8w,0x3f
	shl r8w,4						;(x)

	mov r9,r8						;source
	mov r10,0x03000400				;xres,yres
	mov r11,0x00100010				;xcount,ycount
	mov r12,0x1000000				;sourceaddr
	call updatescreen				;先把上一次改掉的地方还原

.cleanmenu:
	test byte [rel esckey],1
	jnz .skipclean

	;mov r10,0x01000100				;xres,yres
	;mov r11,0x01000100				;xcount,ycount
	;mov r12,0x1300000				;sourceaddr
	;call updatescreen				;先把上一次改掉的地方还原
.skipclean:




.newmouse:
	mov r8,[rel offset]
	shr r8,6
	shl r8,4+16						;(y<<16)
	mov r8w,[rel offset]
	and r8w,0x3f
	shl r8w,4						;(x)
	call mousenew

.newmenu:
	add r8,0x00100010
	call menu




.return:
	ret
;_______________________________________
offsetold:dq 0
offset:dq 0				;“鼠标”位置








;____________________________________
f1foregroundevent:

.up:
	cmp al,0x48
	jne .notup

	dec byte [rel chosen]
	and byte [rel chosen],0x7
	ret
.notup:




.down:
	cmp al,0x50
	jne .notdown

	inc byte [rel chosen]
	and byte [rel chosen],0x7
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




.backspace:
	cmp al,0xe
	jne .notbackspace

	shr qword [rel input],4
	ret
.notbackspace:




.other:
	call scan2hex

	cmp al,0xf					;<0x30
	ja .return

	shl qword [rel input],4
	add [rel input],al




.return:
	ret
;______________________________________
input:dq 0