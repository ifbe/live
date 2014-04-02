bits 64
;_________________________________________________
forever:

hlt                     ;       sleep

cmp byte [0xff0],0      ;       (keyboard interrupt)?
je forever              ;       no{sleep again}
                        ;       yes{
mov byte [0xff0],0      ;               clear signal
mov eax,[0xff8]         ;               pointer=[0xff8]
cmp ax,0x800            ;               (pointer=0x800(buffer head))?
je forever              ;               yes{sleep again}
                        ;               no{
dec eax                 ;                       pointer-1
mov al,[eax]            ;                       al=[pointer]

decide:
jmp function1		;               	it is in charge currently
			;		}
                        ;       }
;_________________________________________________
