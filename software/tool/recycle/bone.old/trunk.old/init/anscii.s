%define ansciihome 0x110000

optimizeanscii:
mov esi,0x4000
mov edi,ansciihome

.continue:
mov bl,[esi]
inc esi

mov ecx,8
.eighttimes:
shl bl,1
jc .color
	.nocolor:
	xor eax,eax
	jmp .put
	.color:
	mov eax,0xffffffff
.put:
stosd
loop .eighttimes

cmp esi,0x4800
jb .continue
