bits 64
section .text
global int20
global hltwait


;_________________________________________________
hltwait:

.compare:
mov rax,[0xff8]
cmp [0xfe8],rax                 ;       [0xff8]=[0xfe8]?
jne .process

mov rax,[rel signal]
mov byte [rel signal],0
cmp rax,0
jnz .return

.wait:
hlt                             ;       sleep
jmp .compare                    ;       waken up

.process:
mov rax,[0xff8]                 ;       pointer=[0xff8]
mov al,[rax]                    ;       return value=[pointer]
inc qword [0xff8]
cmp qword [0xff8],0xfe0
jb .return
mov qword [0xff8],0x800

.return:
ret
;___________________________________________________




;____________________________________
int20:
lea rax,[rel serveroftimer]
mov edi,0x1200
call idtinstaller

mov edi,0xfee00000
mov dword [edi+0x320],0x20020       ;timer vector
mov dword [edi+0x3e0],0x8          ;devide value
mov dword [edi+0x380],0xffffff          ;init value
sti

ret
;___________________________________




;_______________________________
serveroftimer:
push rax

mov byte [rel signal],0xff
mov eax,0xfee000b0
mov dword [rax],0

pop rax
iretq
;________________________________
signal:dq 0



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
