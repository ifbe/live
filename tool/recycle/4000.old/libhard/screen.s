%define screeninfo 0x2000
%define ansciihome 0x30000
[bits 64]




;__________________________________________
;al字母，di显存位置
slowcharacter:
    push rcx

    mov esi,0x3000
	movzx eax,al
    shl eax,4
    add esi,eax
    mov ecx,16

.continuey:
    xor dx,dx
    mov dl,[esi]
    inc esi

.continuex:
    xor rax,rax
    not rax
    shl dl,1
    jc .skip
    xor eax,eax

.skip:
    mov [edi],eax
    add edi,4
    inc dh
    cmp dh,8
    jb .continuex

    add edi,(1024-8)*4
    loop .continuey

.return:
	sub edi,1024*4*16-32
	pop rcx
	ret
;__________________________________________________




;_________________________________
fastcharacter:
character:
	push rdi
    push rcx

    movzx esi,al
    shl esi,9
    add esi,ansciihome
    mov ecx,16
	cld

.lines:		;每排8个DWORD=4个QWORD
	movsq
	movsq
	movsq
	movsq
	add edi,(1024-8)*4
    loop .lines

.return:
    pop rcx
	pop rdi
    add edi,32
    ret
;____________________________________




;__________________________________
char:
	cmp al,0x80
	jae .blank
	cmp al,0x20
	ja .put

.blank:
	mov al,0x20
.put:
	call character
	ret
;____________________________________




;____________________________________
writescreen:
	mov rsi,rbp
	mov edi,[screeninfo+0x28]
	mov bl,[screeninfo+0x19]
	shr bl,3
	movzx ebx,bl
	mov ecx,1024*767
.continuescreen:
	lodsd
	mov [edi],eax
	add edi,ebx
	loop .continuescreen

	ret
;____________________________________




;r8(rdi):dest
;				(z<<32)+(y<<16)+x
;				0x 0000 0200 0100
;				(256,512,0)

;r9(rsi):source
;				(z<<32)+(y<<16)+x
;				0x 0000 0010 0000
;				(0,16,0)

;r10(rdx):length
;				(zres<<32)+(yres<<16)+xres
;				0x 0100 0300 0400
;				1024*768*512

;r11(rcx):count
;				(zcount<<32)+(ycount<<16)+xcount
;				0x 0001 0010 0010
;				16*16*1

;r12(r8):addr
;				rel mousedata

;__________________________________________
updatescreen:			;ebx=byte per pixel
	mov bl,[screeninfo+0x19]
	shr bl,3
	movzx rbx,bl




.destination:					;r8
	mov edi,[screeninfo+0x28]

	;mov rax,r8
	;shr rax,32
	;movzx rax,ax				;rax=z
	;shl rax,10+10				;rax=z*1024*1024
	;imul rbx					;rax=z*1024*1024*bpp
	;add rdi,rax

	mov rax,r8
	shr rax,16
	movzx rax,ax				;rax=y
	shl eax,10					;rax=y*1024
	mul ebx						;rax=y*1024*bpp
	add edi,eax

	mov rax,r8
	movzx rax,ax				;rax=x
	mul ebx						;rax=x*bpp
	add edi,eax




.source:						;r9
	mov rsi,r12

	;mov rax,r9
	;shr rax,32
	;movzx rax,ax
	;......ax*xres*yres
	;shl rax,2
	;add rsi,rax

	mov rax,r9
	shr rax,16
	movzx rax,ax				;rax=y
	mov rcx,r10
	movzx rcx,cx
	mul ecx						;rax=y*xres
	shl eax,2					;rax=y*xres*4
	add esi,eax

	mov rax,r9
	movzx rax,ax				;rax=x
	shl rax,2					;rax=x*4
	add esi,eax




;	for(y=0;y<ycount;y++)			ycount=(r11>>16)&0xffff
;		for(x=0;x<xcount;x++)		xcount=r11&0xffff
;			[rdi + y*1024*bpp + x] = [rsi + y*xres*4 + x*4]
.okdoit:
	mov [rel .updatecount],r11

.continuey:
	movzx rcx,r11w

.continuex:
	;放一行数据
	mov eax,[esi]
	mov [edi],eax
	add esi,4
	add edi,ebx
	loop .continuex

	;下一次的di
	movzx rax,r11w
	mul ebx
	sub edi,eax			;edi -= xcount * bpp
	mov eax,ebx
	shl eax,10
	add edi,eax			;edi += 1024 * bpp

	;下一次的si
	movzx rax,r11w
	shl rax,2
	sub esi,eax			;esi -= xcount * 4
	movzx rax,r10w
	shl rax,2
	add esi,eax			;esi += xres * 4

	;进不进入下一次
	dec word [rel .ycount]
	cmp word [rel .ycount],0
	ja .continuey




.return:
	ret
;__________________________________________
.updatecount:
.xcount:dw 0
.ycount:dw 0
.zcount:dw 0
.reserved:dw 0

























;把一串字符直接显示到edi指定的屏幕位置
;_______________________________
;message:
;whatfuck111:
;	mov ecx,16
;.continue:
;	push rcx
;	push rsi
;	lodsb
;	cmp al,0x20
;	ja .thisdone
;	add edi,32
;	jmp .notprint
;.thisdone:
;	call char
;.notprint:
;	pop rsi
;	pop rcx
;	inc esi
;	loop .continue
;	ret
;______________________________




;把rbx的值直接显示到edi指定的屏幕位置
;_________________________________
;whatfuck2222:
;dumprbx:
;	mov ecx,16
;	mov byte [rel firstnonzero],0
;.getaddress:
;	rol rbx,4
;	mov al,bl
;	and al,0x0f

;	cmp byte [rel firstnonzero],0		;have first non zero?
;	ja .print
;	cmp ecx,1				;last value?
;	je .print

;	cmp al,0				;not print 0
;	je .notprint

;	.setsignal:				;now we have a non zero
;	mov byte [rel firstnonzero],1

;	.print:
;		push rbx
;		push rcx
;		call character
;		pop rcx
;		pop rbx
;	loop .getaddress

;	ret

;	.notprint:
;	add edi,32
;	loop .getaddress

;	ret
;________________________________
;firstnonzero: db 0




;rbp是内存里面rgb图像的地址,临时用rbp传一下以后会改
;r8~r15用来传参,传进来4个都是坐标值(0,0)~(1024,768)
;r8=leftx,r9=rightx,r10=upy,r11=downy
;_____________________________________________
;updateugly:
;.debug:
;	mov [?],r8d
;	mov [?],r9d
;	mov [?],r10d
;	mov [?],r11d




;.check:
;	cmp r8,r9				;if(leftx>=rightx)return
;	jae .return
;	cmp r10,r11				;if(upy>=downy)return
;	jae .return
;	cmp r9,1024				;if(rightx>=1024)return
;	ja .return
;	cmp r11,768				;if(downy>=768)return
;	ja .return




;.getvariety:
;	mov bl,[screeninfo+0x19]
;	shr bl,3
;	movzx ebx,bl				;ebx=byte per pixel




;.continuey:
;	mov edi,[screeninfo+0x28]		;dest base
;	mov eax,ebx
;	shl eax,10
;	mul r10d
;	add edi,eax				;+upx*1024*bpp
;	mov eax,ebx
;	mul r8d
;	add edi,eax				;+leftx*bpp

;	mov rsi,rbp				;source base
;	mov rax,r10
;	shl rax,10+2
;	add rsi,rax				;+upx*1024*4
;	mov rax,r8
;	shl rax,2
;	add rsi,rax				;+leftx*4

;	mov rcx,r9				;rightx
;	sub rcx,r8				;leftx
;.continuex:
;	mov eax,[esi]
;	add esi,4
;	mov [edi],eax
;	add edi,ebx
;	loop .continuex

;	inc r10
;	cmp r10,r11
;	jb .continuey




;.return:
;	ret
;_____________________________________________