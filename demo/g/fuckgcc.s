bits 64
section .text
global int20
global shutup20
global hltwait


;_________________________________________________
hltwait:

hlt                     ;       sleep
mov rax,[rel signal]
mov byte [rel signal],0
cmp rax,0
jnz .leave

cmp byte [0xff0],0      ;       (keyboard wake up me)?
je hltwait              ;       no{sleep again}
                        ;       yes{
mov byte [0xff0],0      ;               clear signal
mov eax,[0xff8]         ;               pointer=[0xff8]
cmp eax,0x800            ;               (pointer=0x800(buffer head))?
je hltwait              ;               yes{sleep again}
                        ;               no{
dec eax                 ;                       pointer-1
mov al,[eax]            ;                       al=[pointer]

.leave:
ret
;___________________________________________________




;____________________________________
int20:
lea rax,[rel serveroftimer]
mov edi,0x1200
call idtinstaller

mov edi,0xfee00000
mov dword [edi+0x320],0x20020       ;timer vector
mov dword [edi+0x3e0],0x1          ;devide value
mov dword [edi+0x380],0xffffff          ;init value
sti

ret
;___________________________________




;_________________________________
shutup20:
mov edi,0xfee00320
mov dword [edi],0x10000
ret
;_________________________________




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
