%define slowanscii 0x3000
%define fastanscii 0x30000
[bits 64]

optimizeanscii:
	mov esi,slowanscii
	mov edi,fastanscii

.continue:
	mov bl,[esi]
	inc esi
	mov ecx,8

.eighttimes:
	xor eax,eax
	shl bl,1
	jnc .skip
.blackcolor:
	not eax
.skip:
	stosd
	loop .eighttimes

	cmp esi,slowanscii+0x800
	jb .continue