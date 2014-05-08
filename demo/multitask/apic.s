[bits 64]
;___________int70h>>int28h_________________
rtc:
lea rax,[rel rtcisr]
mov edi,0x1280
call idtinstaller

mov edi,0xfec00000
mov dword [edi],0x10+     2*8
mov dword [edi+0x10],0x28
mov dword [edi],0x10+1+   2*8
mov dword [edi+0x10],0

xor rax,rax
mov [0x7e0],rax
mov [0x7e8],rax
mov [0x7f0],rax
mov [0x7f8],rax

mov al,0x8a
out 0x70,al
mov al,0xa6           ;3=8192hz,6=1024hz,a=64hz,f=2hz
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
;_____________________________________


jmp kernel


;_______________________________
rtcisr:
push rax
inc qword [0x7f8]       ;信息
mov byte [0x7f0],0xff   ;标志

mov al,0x8c             ;these must be done
out 0x70,al
in al,0x71

mov eax,0xfee000b0
mov dword [eax],0
pop rax
jmp taskswitch
;________________________________



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
