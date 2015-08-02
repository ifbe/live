%define screeninfo 0x1000
[bits 64]




;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-----F1-----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;___________________________
function1:

cmp al,0x3b
je f1
cmp al,0x3c
je f2
cmp al,0x3d
je f3
cmp al,0x3e
je f4
cmp al,0x3f
je f5

cmp al,0x01
je f1esc
cmp al,0xf
je f1tab
cmp al,0x1c
je f1enter
cmp al,0xe
je f1backspace
cmp al,0x4b
je f1left
cmp al,0x4d
je f1right
cmp al,0x48
je f1up
cmp al,0x50
je f1down
;cmp al,0x39
;je f1space

cmp al,0x80
ja ramdump
jmp f1other


f1tab:
cmp byte [rel chosen],6
ja .chosenzero
inc byte [rel chosen]
jmp ramdump
.chosenzero:
mov byte [rel chosen],0
jmp ramdump

f1backspace:
shr qword [rel input],4
jmp ramdump

f1enter:
test byte [rel esckey],1
jnz ramdump
cmp byte [rel chosen],0
je changeaddress
cmp byte [rel chosen],1
je changesector
cmp byte [rel chosen],2
je changeoffset
cmp byte [rel chosen],4
je searchmemory
cmp byte [rel chosen],5
je changememory
cmp byte [rel chosen],6
je changeview
cmp byte [rel chosen],7
je poweroff
jmp ramdump

	;____________________________________
	changeaddress:
	mov rax,[rel input]
	mov rbx,rax
	and rax,0xfffffffffffffc00
	and rbx,0x3ff
	mov [rel addr],rax
	mov [rel offset],rbx
	mov qword [rel input],0
	jmp ramdump
	;___________________________________

	;________________________________________________
	changesector:

	mov rsi,0x180000
	.continue:
	cmp dword [rsi],"read"
	je .findit
	add rsi,0x10
	cmp rsi,0x180000+0x1000
	jb .continue
	jmp ramdump

	.findit:
	mov rdx,[rsi+8]
	mov rdi,[rel input]
	mov [rel sector],rdi
	and rdi,0xfffffffffffffff8
	call rdx

	mov rax,[rel input]
	and rax,0x7
	shl rax,9
	add rax,0x2000000
	mov [rel addr],rax
	mov qword [rel input],0
	jmp ramdump
	;_________________________________________________

	;____________________________________
	changeoffset:
	mov rax,[rel input]
	cmp rax,0xbff
	ja ramdump
	mov [rel offset],rax
	mov qword [rel input],0
	jmp ramdump
	;_____________________________________

	;______________________________________
	searchmemory:
	mov eax,[rel input]
	mov edi,[rel addr]
	mov ebx,0
	.loopsearch:
	cmp [edi+ebx],eax
	je .findit
	inc ebx
	cmp ebx,0xc00
	jb .loopsearch
	jmp ramdump
	.findit:
	mov [rel offset],rbx
	jmp ramdump
	;___________________________________

	;________________________________
	changememory:
	mov eax,[rel input]
	mov rdi,[rel addr]
	add rdi,[rel offset]
	mov [rdi],eax
	mov qword [rel input],0
	jmp ramdump
	;_______________________________

	;__________________________________
	changeview:
	inc byte [rel viewmode]
	test byte [rel viewmode],1
	jnz .anscii
	    .hex:
	    lea rax,[rel dumphex]
	    mov [rel hexoranscii],rax
	    jmp ramdump
	    .anscii:
	    lea rax,[rel dumpanscii]
	    mov [rel hexoranscii],rax
	    jmp ramdump
	;__________________________________
	viewmode:dd 1

f1left:
mov rax,[rel offset]
mov bl,0x40
div bl
cmp ah,0
je .equal
dec qword [rel offset]
jmp ramdump
    .equal:
    cmp qword [rel addr],0x800
    jb ramdump
    sub qword [rel addr],0x800
    jmp ramdump

f1right:
mov rax,[rel offset]
mov bl,0x40
div bl
cmp ah,0x3f
je .equal
inc qword [rel offset]
jmp ramdump
    .equal:
    add qword [rel addr],0x800
    jmp ramdump

f1up:
cmp qword [rel offset],0x40
jb .below
sub qword [rel offset],0x40
jmp ramdump
    .below:
    cmp qword [rel addr],0x40
    jb ramdump
    sub qword [rel addr],0x40
    jmp ramdump

f1down:
cmp qword [rel offset],0xbbf
ja .above
add qword [rel offset],0x40
jmp ramdump
    .above:
    add qword [rel addr],0x40
    jmp ramdump

;f1space:
;    mov qword [rel addr],0x6000
;    mov qword [rel offset],0
;    jmp ramdump

f1esc:
inc byte [rel esckey]
test byte [rel esckey],1
jnz .anscii
    .hex:
    lea rax,[rel menu]
    mov [rel mouseormenu],rax
    jmp ramdump
    .anscii:
    lea rax,[rel mouse]
    mov [rel mouseormenu],rax
    jmp ramdump

f1other:
call scan2hex

cmp al,0xf		;<0x30
ja ramdump

shl qword [rel input],4
add [rel input],al
jmp ramdump

;_______________________________________
esckey db 0




;______________________________
ramdump:

call [rel hexoranscii]
call [rel mouseormenu]

    mov esi,0x1000000
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


jmp forever

;___________________________________
mouseormenu:dq 0
hexoranscii:dq 0




;_____________________________________
dumpanscii:

mov rax,[rel addr]
mov [rel ansciipointer],rax
mov dword [rel ansciicount],0
.ansciiline:
	mov edi,0x1000000			;locate destination
	mov eax,[rel ansciicount]
	imul eax,4*1024*16
	add edi,eax
	call dumpanscii64
	inc byte [rel ansciicount]
	cmp byte [rel ansciicount],0x30
	jb .ansciiline
ret
;______________________________________
ansciipointer:dq 0
ansciicount:dd 0




;_______________________________
dumpanscii64:

mov ecx,0x40
.dump:
    mov al,0x20
    call char

    mov rbx,[rel ansciipointer]
    mov al,[rbx]
    cmp al,0x20
    jb .blank
    cmp al,0x80
    jb .visiable

    .blank:
    mov al,0x20

    .visiable:
    call char
    inc qword [rel ansciipointer]

dec cl
jnz .dump
ret
;_______________________________




;_____________________________________
dumphex:

mov rax,[rel addr]
mov [rel hexpointer],rax
mov dword [rel hexcount],0
.hexline:
	mov edi,0x1000000			;locate destination
	mov eax,[rel hexcount]
	imul eax,4*1024*16
	add edi,eax
	call dumphex64
	inc byte [rel hexcount]
	cmp byte [rel hexcount],0x30
	jb .hexline
ret
;______________________________________
hexpointer:dq 0
hexcount:dd 0




;_______________________________
dumphex64:

mov cl,0x10
.dump4:
mov rax,[rel hexpointer]
mov eax,[rax]
mov [rel pcimust32],eax

  mov ch,4
  .dump:
  mov al,[rel pcimust32]
  shr al,4
  call character
  mov al,[rel pcimust32]
  and al,0xf
  call character
  shr dword [rel pcimust32],8
  dec ch
  cmp ch,0
  jne .dump

add qword [rel hexpointer],4
dec cl
jnz .dump4

ret
;_______________________________
pcimust32:dd 0




;___________________________________
mouse:
mov edi,0x1000000
mov eax,[rel offset]
mov bl,0x40
div bl

movzx ebx,ah           ;余数
imul ebx,32
shl ebx,1
add edi,ebx

movzx eax,al           ;商
imul eax,4*1024*16
add edi,eax

mov ecx,16
.first:
xor edx,edx
	.second:
	not dword [edi+edx]
	add edx,4
	cmp edx,64
	jb .second
add edi,4*1024
loop .first

ret
;___________________________________




;_____________________________________
menu:
call mouse
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


mov rbx,[rel menuaddr]
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


somewords:

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

choose:
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
addr:dq 0
sector:dq 0
offset:dq 0
input:dq 0
