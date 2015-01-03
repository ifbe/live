;old 8259,not used
org 0x10000
[bits 64]
;___________int70h>>int28h_________________
rtc:
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

mov rax,rtcisr
mov edi,0x1280
call idtinstaller

xor rax,rax
mov [0x7e0],rax
mov [0x7e8],rax
mov [0x7f0],rax
mov [0x7f8],rax

in al,0x21
and al,0xfb     ;enable
out 0x21,al
in al,0xa1
and al,0xfe
out 0xa1,al
;__________________________________



jmp kernel


;____________________________________
rtcisr:
push rax
inc qword [0x7f8]       ;信息
mov byte [0x7f0],0xff   ;标志

mov al,0x8c             ;these must be done
out 0x70,al
in al,0x71

mov al,0x20
out 0xa0,al
out 0x20,al
pop rax
jmp taskswitch
;iretq
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
