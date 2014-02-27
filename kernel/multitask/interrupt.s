org 0x10000
[bits 64]
;__________________int3h______________________
breakpoint:
mov rax,breakpointisr
mov edi,0x1030
call idtinstaller

jmp breakpointdone

breakpointisr:
inc qword [0xff20]
iretq

breakpointdone:
;_____________________________________


;_____________int9h>>int21h__________________
keyboard:
mov rax,keyboardisr
mov edi,0x1210
call idtinstaller

xor rax,rax
mov [0xfe0],rax
mov [0xfe8],rax
mov [0xff0],rax
mov [0xff8],rax

in al,0x21
and al,0xfd	;enable
out 0x21,al

jmp keyboarddone

;first
keyboardisr:
push rax
push rbx
in al,0x60
cmp al,0x80
ja .leave

mov byte [0xff0],0xff           ;标志
mov ebx,[0xff8]
mov [ebx],al
inc ebx
cmp bx,0xf00
jb .putback
mov ebx,0x800
.putback:
mov [0xff8],ebx
.leave:
mov al,0x20
out 0x20,al
pop rbx
pop rax

;jmp taskswitch
iretq


keyboarddone:
;________________________________________


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

jmp rtcdone


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

rtcdone:
;_____________________________________


jmp interruptdone


;____________________________________
taskswitch:
;inc byte [0xfffc]
mov [save],rsp
cmp byte [0x800f8],0xff
jne task1

task0:
mov dword [running],0x80000
mov dword [next],0x80100
mov byte [0x800f8],0
mov byte [0x801f8],0xff
jmp switch

task1:
cmp byte [0x801f8],0xff
jne task2

mov dword [running],0x80100
mov dword [next],0x80200
mov byte [0x801f8],0
mov byte [0x802f8],0xff
jmp switch

task2:
mov dword [running],0x80200
mov dword [next],0x80000
mov byte [0x802f8],0
mov byte [0x800f8],0xff

switch:
mov rsp,[running]
add rsp,0xf8
push rbp              ;+0xff0
push rsi
push rdi              ;+0xfe0
push rax
push rcx
push rdx
push rbx              ;+0xfc0
push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15              ;+0xf80

mov rsi,[save]
mov rdi,[running]
cld
movsq
movsq
movsq
movsq
movsq

mov rsp,[next]
add rsp,0x80
pop r15              ;+0xf80
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8
pop rbx              ;+0xfc0
pop rdx
pop rcx
pop rax
pop rdi              ;+0xfe0
pop rsi
pop rbp              ;+0xff0

mov rsp,[next]
.return:
iretq
;____________________________________
save:dq 0
running:dq 0
next:dq 0


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

interruptdone:
