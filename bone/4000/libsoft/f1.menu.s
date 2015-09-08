%define ansciihome 0x30000
[bits 64]




;________________________________________
fixxy:
	add r8,0x00100010

	cmp r8w,1024-256
	jna .skipx
	sub r8w,256+16
.skipx:

	cmp r8,(768-64+16)<<16
	jb .skipy
	sub r8,(64+16)<<16
.skipy:

.return:
	ret
;__________________________________________



	
;__________________________________________
cleanmenu:
	call fixxy
	mov r9,r8
	mov r10,0x03000400				;xres,yres
	mov r11,0x00400100				;xcount,ycount
	mov r12,0x1000000				;sourceaddr
	jmp updatescreen				;先把上一次改掉的地方还原
;___________________________________________




;[0x1300000,0x13fffff]:f1foreground(4*256*64=64KB)
;_____________________________________
menu:
	call fixxy
	push r8

.anscii:
	lea r9,[rel message0]
	mov r8,0x1300000
	call menuanscii16
	mov r8,0x1300000+4*256*16
	call menuanscii16
	mov r8,0x1300000+4*256*16*2
	call menuanscii16
	mov r8,0x1300000+4*256*16*3
	call menuanscii16




.data:
	mov r8,[rel addr]
	lea r9,[rel string]
	call data2hexstring
	mov r8,0x1300000+4*128
	lea r9,[rel string]
	call menuanscii16

	mov r8,[rel offset]
	lea r9,[rel string]
	call data2hexstring
	mov r8,0x1300000+4*128+4*256*16
	lea r9,[rel string]
	call menuanscii16

	mov r8,[rel input]
	lea r9,[rel string]
	call data2hexstring
	mov r8,0x1300000+4*128+4*256*16*2
	lea r9,[rel string]
	call menuanscii16

	mov r8,0xffffffffffffffff
	lea r9,[rel string]
	call data2hexstring
	mov r8,0x1300000+4*128+4*256*16*3
	lea r9,[rel string]
	call menuanscii16




.screen:
	pop r8
	xor r9,r9
	mov r10,0x00400100
	mov r11,0x00400100
	mov r12,0x1300000
	call updatescreen

	ret
;___________________________________________
chosen:dq 0




;r8:destination
;r9:source
;____________________________________
menuanscii:

.nextdata:
    movzx rsi,byte [r9]
    shl rsi,9
    add rsi,ansciihome
	mov rdi,r8
    mov rcx,16
	cld

.lines:		;每排8个DWORD=4个QWORD
	lodsq
	not rax
	stosq
	lodsq
	not rax
	stosq
	lodsq
	not rax
	stosq
	lodsq
	not rax
	stosq
	add rdi,(256-8)*4
	loop .lines

.return:
    add r8,32
	inc r9
    ret
;____________________________________




;____________________________________
menuanscii16:
	call menuanscii		;0
	call menuanscii
	call menuanscii
	call menuanscii
	call menuanscii		;4
	call menuanscii
	call menuanscii
	call menuanscii
	call menuanscii		;8
	call menuanscii
	call menuanscii
	call menuanscii
	call menuanscii		;c
	call menuanscii
	call menuanscii
	call menuanscii
	ret
;_____________________________________




;_____________________________
message0:
dq "address:",0x2020202020202020
dq "offset: ",0x2020202020202020
dq "input:  ",0x2020202020202020
dq "type:   ",0x2020202020202020
;________________________________




;__________________________________________
;menuold:
;.menurow:
;	mov rbx,[rel offset]
;	and rbx,0xffffffffffffffc0
;	shl rbx,10                      ;now ebx=line number * pixel/line
;	add rbx,0x10000
;	cmp rbx,0x200000
;	jna .skiprow
;	sub rbx,0x110000
;.skiprow:

;.menuline:
;	mov rax,[rel offset]
;	and rax,0x3f
;	shl rax,6
;	add rax,0x40
;	cmp rax,0xc00
;	jna .skipline
;	sub rax,0x440
;.skipline:

;	add rbx,rax
;	add rbx,0x1000000               ;framebuffer address
;	mov [rel menuaddr],rbx




;.backcolor:
;	mov edi,ebx
;	mov ecx,0x100
;.innercolor:
;	mov eax,ecx
;	shl eax,15
;	stosd
;	loop .innercolor

;	add ebx,0x1000          ;一行

;	mov eax,ebx
;	sub eax,[rel menuaddr]
;	cmp eax,0x100000
;	jb .backcolor




;.somewords:
;	lea esi,[rel message0]
;	mov dword [rel menuoffset],0x8000
;.entries:
;	mov edi,[rel menuaddr]
;	add edi,[rel menuoffset]
;	call message
;	add dword [rel menuoffset],0x20000
;	cmp dword [rel menuoffset],0x100000
;	jb .entries

;	mov edi,[rel menuaddr]
;	add edi,0x8200
;	mov rbx,[rel addr]
;	call dumprbx
;	mov edi,[rel menuaddr]
;	add edi,0x28200
;	mov rbx,[rel sector]
;	call dumprbx
;	mov edi,[rel menuaddr]
;	add edi,0x48200
;	mov rbx,[rel offset]
;	call dumprbx
;	mov edi,[rel menuaddr]
;	add edi,0x68200
;	mov rbx,[rel input]
;	call dumprbx




;.choose:
;	mov edi,[rel chosen]
;	shl edi,17
;	add edi,[rel menuaddr]
;	mov edx,32

;.loop32:
;	mov ecx,0x100

;.innerloop:
;	not dword [edi]
;	add edi,4
;	dec ecx
;	jnz .innerloop

;	add edi,0xc00
;	dec edx
;	jnz .loop32

;	ret
;_______________________________
;menuaddr dq 0
;menuoffset:dq 0




;_____________________________
;message0:
;db "address:        "
;db "sector:         "
;db "offset:         "
;db "input:          "
;db "search          "
;db "change          "
;db "anscii/hex      "
;db "poweroff        "
;________________________________