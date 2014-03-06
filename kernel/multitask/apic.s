org 0x10000
[bits 64]
;___________int70h>>int28h_________________
rtc:
mov rax,rtcisr
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

jmp rtcdone


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

rtcdone:
;_____________________________________


jmp interruptdone


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


;____________________________________
taskswitch:

.saverunning:
mov [save],rsp
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

.changepointer:
add dword [running],0x100
cmp dword [running],0x80200
jb .releasethis
mov dword [running],0x80000

.releasethis:
mov rsp,[running]
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

mov rsp,[running]
.return:
iretq
;____________________________________
save:dq 0
running:dq 0x80000


interruptdone:
