bits 64
section .text
global keyboard


keyboard:

hlt                     ;       sleep

cmp byte [0xff0],0      ;       (keyboard wake up me)?
je keyboard              ;       no{sleep again}
                        ;       yes{
mov byte [0xff0],0      ;               clear signal
mov eax,[0xff8]         ;               pointer=[0xff8]
cmp eax,0x800            ;               (pointer=0x800(buffer head))?
je keyboard              ;               yes{sleep again}
                        ;               no{
dec eax                 ;                       pointer-1
mov al,[eax]            ;                       al=[pointer]

ret
