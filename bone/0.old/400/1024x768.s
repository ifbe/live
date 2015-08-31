%define vesahome 0x2000

[BITS 16]
startofscreen:




;_________________________________
	mov si,vesahome
searchresolution:
	add si,0x20
	cmp si,vesahome+0xfff
	ja endofscreen
	mov eax,[si+0x12]
	cmp eax,0x03000400
	jne searchresolution
	cmp byte [si+0x19],0x18
	jb searchresolution
;__________________________________




;__________________________________
setresolution:
	sub si,vesahome
	shr si,5
	push si

	mov cx,si
	mov ch,1
	mov di,vesahome
	mov ax,0x4f01
	int 0x10

	pop bx
	mov bh,0x41
	mov ax,0x4f02
	int 0x10
;__________________________________
	jmp endofscreen




paddingofscreen:
times 0x400-(paddingofscreen-startofscreen) db 0



endofscreen:
