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

cmp al,0x48
je escup
cmp al,0x50
je escdown
cmp al,0x1c
je escenter

jmp esc


escup:
cmp byte [rel chosen],1
jna esc
dec byte [rel chosen]
jmp esc

escdown:
cmp byte [rel chosen],4
jae esc
inc byte [rel chosen]
jmp esc

escother:
mov byte [rel temp],0

escenter:
cmp byte [rel chosen],3
je decide
cmp byte [rel chosen],4
je turnoff
jmp esc




menu:
mov edx,0x1100000	;屏幕256行
mov ebx,0x700		;384个像素点
.backcolor:
lea edi,[edx+ebx]
mov ecx,0x80
mov eax,0x22222222
rep stosd
add ebx,0x1000		;一行
cmp ebx,0x100700
jb .backcolor

lea esi,[rel message0]
mov edi,0x1100700
call message
lea esi,[rel message1]
mov edi,0x1120700
call message
mov edi,0x1140700
call message
mov edi,0x1160700
call message
mov edi,0x1180700
call message

choose:
mov edi,[rel chosen]
shl edi,17
add edi,0x1100700
mov ecx,0x80
mov eax,0xff00
rep stosd
add edi,0x1fe00
mov ecx,0x80
rep stosd

mov ecx,0x20
sub edi,0x200
.loop32:
sub edi,0x1000
mov [edi],eax
mov [edi+0x1fc],eax
loop .loop32
ret

chosen:dq 3

message0:db ">>    menu    <<"
message1:db "change address: "
message2:db "change offset:  "
message5:db "     return     "
message6:db "    turnoff!    "
