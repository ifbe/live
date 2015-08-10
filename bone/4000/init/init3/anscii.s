%define slowdata 0x4000
%define ansciihome 0x30000

optimizeanscii:
	mov esi,slowdata
	mov edi,ansciihome

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

	cmp esi,0x4800
	jb .continue
