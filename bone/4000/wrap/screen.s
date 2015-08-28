%define screeninfo 0x1000
%define ansciihome 0x30000
[bits 64]




;_________________________________
character:
    push rcx

    movzx eax,al
    shl eax,9
    lea esi,[eax+ansciihome]

    xor ecx,ecx
.lines:		;每排8个DWORD=4个QWORD
    lodsq
    mov [edi+ecx],rax
    lodsq
    mov [edi+ecx+8],rax
    lodsq
    mov [edi+ecx+16],rax
    lodsq
    mov [edi+ecx+24],rax

    add ecx,1024*4
    cmp ecx,1024*4*16
    jna .lines

.return:
    add edi,32
    pop rcx
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




;把一串字符直接显示到edi指定的屏幕位置
;_______________________________
message:
	mov ecx,16
.continue:
	push rcx
	push rsi
	lodsb
	cmp al,0x20
	ja .thisdone
	add edi,32
	jmp .notprint
.thisdone:
	call char
.notprint:
	pop rsi
	pop rcx
	inc esi
	loop .continue
	ret
;______________________________




;把rbx的值直接显示到edi指定的屏幕位置
;_________________________________
dumprbx:
	mov ecx,16
	mov byte [rel firstnonzero],0
.getaddress:
	rol rbx,4
	mov al,bl
	and al,0x0f

	cmp byte [rel firstnonzero],0		;have first non zero?
	ja .print
	cmp ecx,1				;last value?
	je .print

	cmp al,0				;not print 0
	je .notprint

	.setsignal:				;now we have a non zero
	mov byte [rel firstnonzero],1

	.print:
		push rbx
		push rcx
		call character
		pop rcx
		pop rbx
	loop .getaddress

	ret

	.notprint:
	add edi,32
	loop .getaddress

	ret
;________________________________
firstnonzero: db 0




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




;rbp是内存里面rgb图像的地址,临时用rbp传一下以后会改
;r8~r15用来传参,传进来4个都是坐标值(0,0)~(1024,768)
;r8=leftx,r9=rightx,r10=upy,r11=downy
;_____________________________________________
updatescreen:
.debug:
	mov [0x600],r8d
	mov [0x604],r9d
	mov [0x608],r10d
	mov [0x60c],r11d

.check:
	cmp r8,r9				;if(leftx>=rightx)return
	jae .return
	cmp r10,r11				;if(upy>=downy)return
	jae .return
	cmp r9,1024				;if(rightx>=1024)return
	jae .return
	cmp r11,768				;if(downy>=768)return
	jae .return

.getvariety:
	mov bl,[screeninfo+0x19]
	shr bl,3
	movzx ebx,bl				;ebx=byte per pixel

.continuey:
	mov edi,[screeninfo+0x28]		;dest base
	mov eax,ebx
	shl eax,10
	mul r10d
	add edi,eax				;+upx*1024*bpp
	mov eax,ebx
	mul r8d
	add edi,eax				;+leftx*bpp

	mov rsi,rbp				;source base
	mov rax,r10
	shl rax,10+2
	add rsi,rax				;+upx*1024*4
	mov rax,r8
	shl rax,2
	add rsi,rax				;+leftx*4

	mov rcx,r9				;rightx
	sub rcx,r8				;leftx
.continuex:
	mov eax,[esi]
	add esi,4
	mov [edi],eax
	add edi,ebx
	loop .continuex

	inc r10
	cmp r10,r11
	jb .continuey

.return:
	ret
;______________________________________________
