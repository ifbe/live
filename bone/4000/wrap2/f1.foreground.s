[bits 64]




;_____________________________________
menu:

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
	dec ecx
	jnz .innercolor

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




;____________________________________
f1foregroundevent:

.up:
	cmp al,0x48
	jne .notup

	dec byte [rel chosen]
	and byte [rel chosen],0x7
	ret
.notup:




.down:
	cmp al,0x50
	jne .notdown

	inc byte [rel chosen]
	and byte [rel chosen],0x7
	ret
.notdown:




.enter:
	cmp al,0x1c
	jne .notenter

	test byte [rel esckey],1
	jnz .return
	cmp byte [rel chosen],0
	je changeaddress
	cmp byte [rel chosen],1
	je changesector
	cmp byte [rel chosen],2
	je changeoffset
	cmp byte [rel chosen],4
	je searchinthisscreen
	cmp byte [rel chosen],5
	je changememory
	cmp byte [rel chosen],6
	je changeview
	cmp byte [rel chosen],7
	je poweroff
	ret
.notenter:




.backspace:
	cmp al,0xe
	jne .notbackspace

	shr qword [rel input],4
	ret
.notbackspace:




.other:
	call scan2hex

	cmp al,0xf					;<0x30
	ja .return

	shl qword [rel input],4
	add [rel input],al




.return:
	ret
;______________________________________
input:dq 0




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