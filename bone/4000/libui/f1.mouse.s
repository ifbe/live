[bits 64]




;_____________________________________________
cleanmouse:
	mov r9,r8						;source
	mov r10,0x03000400				;xres,yres
	mov r11,0x00100010				;xcount,ycount
	mov r12,0x1000000				;sourceaddr
	jmp updatescreen				;先把上一次改掉的地方还原
;___________________________________________




;r8=(z<<32)+(y<<16)+x
;______________________________________________
mousenew:
.getnew:							;把这一次的取出来反色然后直接扔上屏幕
	mov rsi,0x1000000

	mov rax,r8
	shr rax,16
	shl rax,10+2					;y*1024*4
	add rsi,rax

	movzx rax,r8w
	shl rax,2						;x*4
	add rsi,rax

	lea edi,[rel mousedata]
	mov ecx,16
	cld
.getcontinue:
	movsq
	movsq
	movsq
	movsq
	movsq
	movsq
	movsq
	movsq
	add esi,4*(1024-16)
	loop .getcontinue




.colorreverse:						;反色
	mov ecx,16*16
	lea edi,[rel mousedata]
.colorcontinue:
	not dword [edi]
	add edi,4
	loop .colorcontinue




.putnew:							;把这一次的颜色写进去
	;r8
	mov r9,0						;sourcex,y
	mov r10,0x00100010				;xres,yres
	mov r11,0x00100010				;xcount,ycount
	lea r12,[rel mousedata]			;sourceaddr

	jmp updatescreen
;___________________________________
mousedata:times 4*16*16 db 0				;0x400