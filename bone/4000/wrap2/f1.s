%define journalhome 0xd00000
[bits 64]




;_________________清空/journal________________
f1init:
	mov qword [rel addr],journalhome		;r14 memory pointer
	mov qword [rel offset],0x420			;r15 offset pointer

	lea rax,[rel dumpanscii]
	mov [rel hexoranscii],rax

	ret
;_______________________________________________








;___________________________________
mouse:




.restoreold:						;把上一次的颜色写回去
	;mov rax,[rel offset]

	;mov r8,rax
	;shr r8,6						;每行64个，所以这里算出第几行
	;shl r8,4+16					;(y<<16)

	;and rax,0x3f
	;shl rax,4
	;add r8,rax						;(y<<16)+x

	;mov r9,r8						;source
	;mov r10,0x03000400				;xres,yres
	;mov r11,0x00100010				;xcount,ycount
	;mov r12,0x1000000				;sourceaddr
	;call updatearea				;先把上一次改掉的地方还原




.getnew:							;把这一次的取出来反色然后直接扔上屏幕
	mov esi,0x1000000

	mov eax,[rel offset]
	shr eax,6						;商
	shl eax,10+2+4
	add esi,eax

	mov eax,[rel offset]
	and eax,0x3f					;余数
	shl eax,6
	add esi,eax						;现在edi是当前“鼠标”位置

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
	mov rax,[rel offset]

	mov r8,rax
	shr r8,6						;每行64个，所以这里算出第几行
	shl r8,4+16						;(y<<16)

	and rax,0x3f
	shl rax,4
	add r8,rax						;(y<<16)+x

	mov r9,0						;sourcex,y
	mov r10,0x00100010				;xres,yres
	mov r11,0x00100010				;xcount,ycount
	lea r12,[rel mousedata]			;sourceaddr
	call updatearea




.return:
	ret
;___________________________________
mousedata:times 4*16*16 db 0				;0x400








;_________________________________________________
f1show:




.background:
	cmp dword [rel f1changebg],0xffff
	jb .skipbackground

	mov dword [rel f1changebg],0
	call [rel hexoranscii]

    mov rbp,0x1000000				;[16m,20m)
	call writescreen
.skipbackground:




.foreground:
	test byte [rel esckey],1
	jnz .skipforeground

	call menu
.skipforeground:



.return:
	call mouse					;再把新的鼠标数据写到当前位置
	ret
;_____________________________________________
hexoranscii:dq 0

f1changebg:dq 0			;
f1changefg:dq 0

addr:dq 0				;当前页面位置
sector:dq 0				;扇区号
offset:dq 0				;“鼠标”位置

esckey:db 0				;显不显示menu
tabkey:db 0				;dumpanscii还是dumphex



 




;_________________________________________________
f1event:
	cmp al,0x80
	jae .return




.tab:
	cmp al,0xf
	jne .nottab

	mov dword [rel f1changebg],0xffff

	inc byte [rel tabkey]
	test byte [rel tabkey],1
	jz .anscii
.hex:
	lea rax,[rel dumphex]
	mov [rel hexoranscii],rax
	ret
.anscii:
	lea rax,[rel dumpanscii]
	mov [rel hexoranscii],rax
	ret
.nottab:




.esc:
	cmp al,0x01
	jne .notesc

	mov dword [rel f1changefg],0xffff

	inc byte [rel esckey]
	ret
.notesc:




.other:
	test byte [rel esckey],1
	jnz f1backgroundevent
	jmp f1foregroundevent




.return:
	ret
;____________________________________________________