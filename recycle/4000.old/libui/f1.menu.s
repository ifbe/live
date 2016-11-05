%define ansciihome 0x30000
[bits 64]




;________________________________________
fixxy:
	add r8,0x00100010

	cmp r8w,1024-256
	jna .skipx
	sub r8w,256+16
.skipx:

	cmp r8,(768-128+16)<<16
	jb .skipy
	sub r8,(128+16)<<16
.skipy:

.return:
	ret
;__________________________________________



	
;__________________________________________
cleanmenu:
	call fixxy
	mov r9,r8
	mov r10,0x03000400				;xres,yres
	mov r11,0x00800100				;xcount,ycount
	mov r12,0x1000000				;sourceaddr
	jmp updatescreen				;先把上一次改掉的地方还原
;___________________________________________




;[0x1300000,0x13fffff]:f1foreground(4*256*64=64KB)
;_____________________________________
menu:
	call fixxy
	push r8




.data:
	mov rax,"/dev/sda"			;/dev/sda
	lea rdi,[rel message0]
	add rdi,0x10
	mov [rdi],rax

	mov r8,[rel addr]			;base
	lea r9,[rel message0]
	add r9,0x30
	call data2hexstring

	mov r8,[rel offset]			;offset
	lea r9,[rel message0]
	add r9,0x50
	call data2hexstring

	mov r8,0					;data
	lea r9,[rel message0]
	add r9,0x70
	call data2hexstring




.selfscreen:
	lea r9,[rel message0]
	mov r8,0x1300000
	call menuanscii32
	mov r8,0x1300000+4*256*16
	call menuanscii32
	mov r8,0x1300000+4*256*16*2
	call menuanscii32
	mov r8,0x1300000+4*256*16*3
	call menuanscii32
	mov r8,0x1300000+4*256*16*4
	call menuanscii32
	mov r8,0x1300000+4*256*16*5
	call menuanscii32
	mov r8,0x1300000+4*256*16*6
	call menuanscii32
	mov r8,0x1300000+4*256*16*7
	call menuanscii32




.screen:
	pop r8
	xor r9,r9
	mov r10,0x00800100
	mov r11,0x00800100
	mov r12,0x1300000
	call updatescreen

	ret
;___________________________________________




;____________________________________________
menuanscii32:
	mov byte [rel .counter],0

.continue:
	call menuanscii
	inc byte [rel .counter]
	cmp byte [rel .counter],32
	jb .continue

.return:
	ret
;______________________________________________
.counter:dq 0




;r8:destination
;r9:source
;____________________________________
menuanscii:
    movzx rax,byte [r9]
	cmp rax,0x20
	jae .skipif0
	mov rax,0x20
.skipif0:
    shl rax,9

.this:
    mov rsi,ansciihome
	add rsi,rax
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




;_____________________________
message0:
	dq "target: ","        ","        ","        "
	dq "base:   ","        ","        ","        "
	dq "offset: ","        ","        ","        "
	dq "data:   ","        ","        ","        "
	dq 0,0,0,0
	dq 0,0,0,0
	dq 0,0,0,0
input:
	dq 0,0,0,0
;________________________________
inputcount:dq 0




;______________________________________________
f4menumessage:

.enter:
	cmp al,0x1c
	jne .notenter

	;jmp changeaddress
	call f1explain
	mov dword [rel offsetold],0xfffff

	ret
.notenter:




.backspace:
	cmp al,0xe
	jne .notbackspace

	cmp byte [rel inputcount],0
	je .dobackspace
	dec byte [rel inputcount]

.dobackspace:
	;shr qword [rel input],4
	lea rbx,[rel input]
	add rbx,[rel inputcount]
	mov qword [rbx],0

	mov rax,[rel offset]
	mov [rel offsetold],rax

	ret
.notbackspace:




.other:
	cmp byte [rel inputcount],0x10
	jae .return

.doinput:
	call scan2anscii
	lea rbx,[rel input]
	add rbx,[rel inputcount]
	mov [rbx],al
	inc byte [rel inputcount]

	mov rax,[rel offset]
	mov [rel offsetold],rax

.return:
	ret
;__________________________________________
	;call scan2hex

	;cmp al,0xf					;<0x30
	;ja .return

	;shl qword [rel input],4
	;add [rel input],al

	;mov rax,[rel offset]
	;mov [rel offsetold],rax

	;cmp byte [rel chosen],1
	;je changesector
	;cmp byte [rel chosen],2
	;je changeoffset
	;cmp byte [rel chosen],4
	;je searchinthisscreen
	;cmp byte [rel chosen],5
	;je changememory
	;cmp byte [rel chosen],6
	;je changeview
	;cmp byte [rel chosen],7
	;je poweroff

























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