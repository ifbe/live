esc:
call menu
call writescreen

hlt			;       sleep
cmp byte [0xff0],0	;       (keyboard wake up me)?
je esc			;       no{sleep again}
			;       yes{
mov byte [0xff0],0	;               clear signal
mov eax,[0xff8]		;               pointer=[0xff8]
cmp ax,0x800		;               (pointer=0x800(buffer head))?
je esc			;               yes{sleep again}
			;               no{
dec eax			;                       pointer-1
mov al,[eax]		;                       al=keyboard message
			;			do as below
			;		}

cmp al,0x1
je escesc
cmp al,0x48
je escup
cmp al,0x50
je escdown
cmp al,0x1c
je escenter
cmp al,0xe
je escbackspace
jmp escother


escesc:
xor eax,eax
jmp decide

escup:
cmp byte [rel chosen],0
je esc
dec byte [rel chosen]
jmp esc

escdown:
cmp byte [rel chosen],6
ja esc
inc byte [rel chosen]
jmp esc

escbackspace:
shr qword [rel receivedvalue],4
jmp esc

escenter:
cmp byte [rel chosen],7
je turnoff

escother:
call scan2hex
cmp al,0x10
ja esc
shl qword [rel receivedvalue],4
add [rel receivedvalue],al
jmp esc




menu:

mov edx,0x1100000	;屏幕256行
mov ebx,0x600		;384个像素点
.backcolor:

lea edi,[edx+ebx]
mov ecx,0x100
.innercolor:
mov eax,ecx
shl eax,1
stosd
dec ecx
jnz .innercolor

add ebx,0x1000		;一行
cmp ebx,0x100600
jb .backcolor


somewords:
lea esi,[rel message0]
mov edi,0x1100600
call message
mov edi,0x1100800
call message
mov edi,0x1120600
call message
mov edi,0x1120800
call message
mov edi,0x11e0700
call message

mov edi,0x1110600
mov rbx,[rel addr]
call dumprbx
mov rbx,[rel receivedvalue]
call dumprbx
mov edi,0x1130600
mov rbx,[rel offset]
call dumprbx


choose:
mov edi,[rel chosen]
shl edi,17
add edi,0x1100600
mov eax,0xff00
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

receivedvalue:dq 0
chosen:dq 0

message0:
db "1.  address:    "
db "->  address:    "
db "2.  offset:     "
db "->  offset:     "
db "    turnoff!    "
