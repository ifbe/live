bits 64
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-----F3----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
function3:

cmp al,0x3b
je f1
cmp al,0x3c
je f2
cmp al,0x3d
je f3
cmp al,0x3e
je f4
cmp al,0x3f
je f4

cyberspace:
	mov edi,0x1800000
	mov ecx,0xc0000
	mov rax,0x88888888
	rep stosd

	mov esi,0x1800000
	mov edi,[0x3028]
	mov bl,[0x3019]
	shr bl,3
	movzx ebx,bl
	mov ecx,1024*768
.continue:
	lodsd
	mov [edi],eax
	add edi,ebx
	loop .continue


jmp forever
