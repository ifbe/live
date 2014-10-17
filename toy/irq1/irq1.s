[bits 64]

;_____________int21h__________________
keyboard:
lea rax,[rel serverofkeyboard]
mov edi,0x1210
call idtinstaller

;enable
mov edi,0xfec00000
mov dword [edi],0x10+        2*1
mov dword [edi+0x10],0x21
mov dword [edi],0x10+1+      2*1
mov dword [edi+0x10],0
;_____________________________________


sti
death:hlt
jmp death


;______________________________________
serverofkeyboard:
in al,0x60                    ;this must be done
call changecolor

mov edi,0xfee000b0
mov dword [edi],0

iretq
;__________________________________


;___________________________________
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

