[bits 64]
section .text
global start
global port
extern clock


;___________int28h_________________
start:
lea rax,[rel serverofrtc]
mov edi,0x1280
call idtinstaller

mov edi,0xfec00000
mov dword [edi],0x10+     2*8
mov dword [edi+0x10],0x28
mov dword [edi],0x10+1+   2*8
mov dword [edi+0x10],0

mov al,0x8a
out 0x70,al
mov al,0xaf           ;3=8192hz,6=1024hz,c=8hz,f=1hz
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

call clock

mov edi,0xfee000b0
mov dword [edi],0

iretq
;_____________________________________


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




port:
mov al,dil
out 0x70,al
in al,0x71
ret
