org 0x10000
[bits 64]
;___________int28h_________________
mov rax,serverofrtc
mov edi,0x1280
call idtinstaller

mov edi,0xfec00000
mov dword [edi],0x10+     2*8
mov dword [edi+0x10],0x28
mov dword [edi],0x10+1+   2*8
mov dword [edi+0x10],0

mov al,0x8a
out 0x70,al
mov al,0xac           ;3=8192hz,6=1024hz,c=8hz,f=1hz
out 0x71,al

mov al,0x8b
out 0x70,al
in al,0x71

mov ah,al
or ah,0x40

mov al,0x8b
out 0x70,al
mov al,ah
out 0x71,al
;_______________________________


sti
death:hlt
jmp death


;____________________________________
serverofrtc:
mov al,0x8c             ;these must be done
out 0x70,al
in al,0x71

call changecolor

mov edi,0xfee000b0
mov dword [edi],0

iretq
;_____________________________________


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
;_________________________________
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


