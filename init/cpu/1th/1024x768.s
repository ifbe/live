BITS 16


startofscreen:
;_________________________________
	mov si,0x3000
searchresolution:
	add si,0x20
	cmp si,0x3fff
	ja endofscreen
	mov eax,[si+0x12]
	cmp eax,0x03000400
	jne searchresolution
	cmp byte [si+0x19],0x18
	jb searchresolution
;__________________________________




;__________________________________
setresolution:
	sub si,0x3000
	shr si,5
	push si

	mov cx,si
	mov ch,1
	mov di,0x3000
	mov ax,0x4f01
	int 0x10

	pop bx
	mov bh,0x41
	mov ax,0x4f02
	int 0x10
;__________________________________
	jmp endofscreen




paddingofscreen:
times 0x200-(paddingofscreen-startofscreen) db 0

endofscreen:
