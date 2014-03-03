org 0x10000
[bits 64]
;_____________apic timer_____________________
;mov edi,0xfee00000
;mov dword [edi+0x320],0x20           ;timer vector
;mov dword [edi+0x3e0],3                ;devide value
;mov dword [edi+0x380],0xffffff          ;init value
;____________________________________


;_____________int 21h__________________
;keyboard:
mov rax,keyboardisr
mov edi,0x1210
call idtinstaller

;enable
mov edi,0xfec00000
mov dword [edi],0x10+2*0x1
mov dword [edi+0x10],0x21
mov dword [edi],0x10+2*0x1+1
mov dword [edi+0x10],0

jmp endofkeyboard


keyboardisr:
push rax
push rbx
in al,0x60
cmp al,0x80
ja .leave

mov byte [0xff0],0xff           ;标志
mov ebx,[0xff8]         ;信息
mov [ebx],al
inc ebx
cmp bx,0xfe0
jb .putback
mov ebx,0x800
.putback:
mov [0xff8],ebx
.leave:
;eoi
mov eax,0xfee000b0
mov dword [eax],0
pop rbx
pop rax
iretq

endofkeyboard:
;________________________________________


jmp endofinterrupt

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


endofinterrupt:
sti
