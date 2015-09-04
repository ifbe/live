;[0x1000000,0x12fffff]:f1background(4*1024*768=3MB)
;[0x1300000,0x13fffff]:f1foreground(4*256*256=256KB)
[bits 64]




;_____________________________________
menu:
	mov edi,0x1300000
	mov ecx,256*256
	mov eax,0x44556677
	cld
	rep stosd

	xor r9,r9
	mov r10,0x01000100
	mov r11,0x01000100
	mov r12,0x1300000
	call updatescreen

	ret

.menurow:
	mov rbx,[rel offset]
	and rbx,0xffffffffffffffc0
	shl rbx,10                      ;now ebx=line number * pixel/line
	add rbx,0x10000
	cmp rbx,0x200000
	jna .skiprow
	sub rbx,0x110000
.skiprow:

.menuline:
	mov rax,[rel offset]
	and rax,0x3f
	shl rax,6
	add rax,0x40
	cmp rax,0xc00
	jna .skipline
	sub rax,0x440
.skipline:

	add rbx,rax
	add rbx,0x1000000               ;framebuffer address
	mov [rel menuaddr],rbx




.backcolor:
	mov edi,ebx
	mov ecx,0x100
.innercolor:
	mov eax,ecx
	shl eax,15
	stosd
	loop .innercolor

	add ebx,0x1000          ;一行

	mov eax,ebx
	sub eax,[rel menuaddr]
	cmp eax,0x100000
	jb .backcolor




.somewords:
	lea esi,[rel message0]
	mov dword [rel menuoffset],0x8000
.entries:
	mov edi,[rel menuaddr]
	add edi,[rel menuoffset]
	call message
	add dword [rel menuoffset],0x20000
	cmp dword [rel menuoffset],0x100000
	jb .entries

	mov edi,[rel menuaddr]
	add edi,0x8200
	mov rbx,[rel addr]
	call dumprbx
	mov edi,[rel menuaddr]
	add edi,0x28200
	mov rbx,[rel sector]
	call dumprbx
	mov edi,[rel menuaddr]
	add edi,0x48200
	mov rbx,[rel offset]
	call dumprbx
	mov edi,[rel menuaddr]
	add edi,0x68200
	mov rbx,[rel input]
	call dumprbx




.choose:
	mov edi,[rel chosen]
	shl edi,17
	add edi,[rel menuaddr]
	mov edx,32

.loop32:
	mov ecx,0x100

.innerloop:
	not dword [edi]
	add edi,4
	dec ecx
	jnz .innerloop

	add edi,0xc00
	dec edx
	jnz .loop32

	ret
;_______________________________
chosen:dq 0
menuaddr dq 0
menuoffset:dq 0




;_____________________________
message0:
db "address:        "
db "sector:         "
db "offset:         "
db "input:          "
db "search          "
db "change          "
db "anscii/hex      "
db "poweroff        "
;________________________________