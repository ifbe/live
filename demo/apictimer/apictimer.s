[bits 64]
;________________________________
lea rax,[rel serveroftimer]
mov edi,0x1200
call idtinstaller

mov edi,0xfee00000
mov dword [edi+0x320],0x20020	;timer vector
mov dword [edi+0x3e0],0x3	;devide 0:2 1:4 2:8 3:16 8:32 9:64 a:128 b:1
mov dword [edi+0x380],0xffffff	;init value
sti
;______________________________

death:hlt
jmp death


;_________________________________
serveroftimer:
call changecolor
mov edi,0xfee000b0
mov dword [edi],0
iretq
;______________________________


;____________________________________
changecolor:
rol dword [rel color],1
mov eax,[rel color]
mov bl,[0x3019]
shr bl,3
movzx ebx,bl
mov ecx,0x80000
mov edi,[0x3028]

.next:
mov [edi],eax
add edi,ebx
loop .next

ret
;________________________________
color:dd 0xff


;___________________________________
idtinstaller:
stosw
mov dword [edi],0x8e000008
add edi,4
shr rax,16
stosw
shr rax,16
stosd
xor eax,eax
stosd
ret
;____________________________________

