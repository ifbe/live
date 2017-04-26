%define vesahome 0x2000

[BITS 16]
startofscreen:




;__________________________________
	mov cx,0x100
searchresolution:
	inc cx
	cmp cx,0x180
	jae endofscreen

	mov di,vesainfo
	mov ax,0x4f01
	int 0x10

	mov eax,[vesainfo+0x12]
	cmp eax,0x03000400
	jne searchresolution

	cmp byte [vesainfo+0x19],0x18
	jb searchresolution

setresolution:
	mov bx,cx
	mov bh,0x41
	mov ax,0x4f02
	int 0x10

	mov eax,[vesahome+0x28]
	mov [vesahome],eax

	mov al,[vesahome+0x19]
	shr al,3
	mov [vesahome+8],al

	mov word [vesahome+0x10],1024
	mov word [vesahome+0x18],768
	jmp endofscreen




paddingofscreen:
times 0x400-(paddingofscreen-startofscreen) db 0
endofscreen:
