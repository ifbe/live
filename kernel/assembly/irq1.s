org 0x10000
[bits 64]

;_____________int9h>>int21h__________________
keyboard:
mov rax,keyboardisr
;mov edi,0x1090
mov edi,0x1210
call idtinstaller

xor rax,rax
mov [0xfe0],rax
mov [0xfe8],rax
mov [0xff0],rax
mov [0xff8],rax

in al,0x21
and al,0xfd     ;enable
out 0x21,al

jmp keyboarddone

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
mov al,0x20
out 0x20,al
pop rbx
pop rax
iretq

keyboarddone:
;________________________________________

death:hlt
jmp death
