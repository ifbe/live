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
	;test byte [rel esckey],1
	;jnz .skipclean

	add r8,0x00100010
	mov r9,r8
	mov r10,0x03000400				;xres,yres
	mov r11,0x00400100				;xcount,ycount
	mov r12,0x1000000				;sourceaddr
	call updatescreen				;先把上一次改掉的地方还原
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