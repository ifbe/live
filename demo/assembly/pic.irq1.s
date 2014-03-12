org 0x10000
[bits 64]

;_____________int21h__________________
keyboard:
mov rax,serverofkeyboard
mov edi,0x1210
call idtinstaller

;enable
in al,0x21
and al,0xfd
out 0x21,al
;_____________________________________


sti
death:hlt
jmp death


;______________________________________
serverofkeyboard:
in al,0x60                    ;this must be done
call changecolor

mov al,0x20
out 0x20,al

iretq
;__________________________________


;___________________________________
changecolor:
rol dword [color],1
mov eax,[color]
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
;________________________________________
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


