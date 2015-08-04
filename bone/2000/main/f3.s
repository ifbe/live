%define screeninfo 0x1000
[bits 64]




;___________________cyberspace______________________
f3show:
	mov edi,0x1800000
	mov ecx,0xc0000
	mov rax,0x88888888
	rep stosd

	mov esi,0x1800000
	mov edi,[screeninfo+0x28]
	mov bl,[screeninfo+0x19]
	shr bl,3
	movzx ebx,bl
	mov ecx,1024*768
.continue:
	lodsd
	mov [edi],eax
	add edi,ebx
	loop .continue

	ret
;_______________________________________________________




;_________________________________________________________
f3event:
	ret
;_________________________________________________________
