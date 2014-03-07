[BITS 64]
;____________________________________
jpegprogram:
    mov esi,0x20000
    mov edi,0x100000
    mov ecx,0x2000
    rep movsq

jpegdecode:                   ;位置无关
    cli
    mov rdx,0x100000            ;eax 传参
    call rdx
    mov [jpegbase],rax        ;return value
    sti

prepare:
    mov r14,0xa000             ;r14 memory pointer
    xor r15,r15             ;r15 offset pointer

    mov rdi,0x800
    mov [0xff8],rdi

    xor rax,rax
    mov [0xff0],rax         ;keyboard buffer end

    mov ecx,0xfe
    rep stosq

    call ramdump
;_______________________________________




;_____________________________________
forever:

hlt			;	sleep

cmp byte [0xff0],0	;	(keyboard interrupt)?
je forever		;	no{sleep again}

			;	yes{
mov byte [0xff0],0	;		clear signal
mov eax,[0xff8]		;		pointer=[0xff8]
cmp ax,0x800		;		(pointer=0x800(buffer head))?
je forever		;		yes{sleep again}
			;		no{
dec eax			;			pointer-1
mov al,[eax]		;			al=[pointer]

switchkey:
			;			(al)?
cmp al,0x3b		;			{
je f1			;			0x3b:f1
cmp al,0x3c
je f2			;			0x3c:f2
cmp al,0x3d
je f3			;			0x3d:f3
cmp al,0x3e
je f4			;			0x3e:f4
cmp al,0x01
je esc			;			0x01:esc
;cmp al,0x1d
;je ctrl			;			0x1d:ctrl
;cmp al,0x38
;je alt			;			0x38:alt
;cmp al,0x3a
;je capslock		;			0x38:capslock
;cmp al,0x2a
;je shift		;			0x38:shift
;cmp al,0x0f
;je tab			;			0x0f:tab
;cmp al,0x5b
;je super		;			0x0f:super

decide:
jmp function1
			;			}
			;		}
			;	}
;_______________________________




;_________________________
f1:
    mov dword [decide+1],function1-(decide+5)		;selfmodify
    jmp function1
f2:
    mov dword [decide+1],function2-(decide+5)		;selfmodify
    jmp function2
f3:
    mov dword [decide+1],function3-(decide+5)		;selfmodify
    jmp function3
f4:
    mov dword [decide+1],function4-(decide+5)		;selfmodify
    jmp function4


esc:
turnoff:
mov dx,[0x4fc]
mov ax,[0x4fe]
or ax,0x2000
out dx,ax

;__________________________




;进:esi,edi
;出:esi,edi
;用:
;变:
;______________________________
char:
    push rbx
    push rcx

    movzx eax,al
    shl ax,4
    lea esi,[eax+0xb000]

    mov ecx,16           ;16行
.yihang:
    xor rax,rax
    lodsb
    push rcx
        mov cl,8
      .yidian:
        shl al,1
        jnc .bg
        mov edx,[frontcolor]
        mov [edi],edx
        jmp .pointdone
      .bg:
        mov edx,[backcolor]
        cmp edx,0xffffffff
        je .pointdone
        mov dword [edi],edx
      .pointdone:
        add edi,4
        loop .yidian
    sub edi,32           ;每个字的行头
    add edi,1024*4            ;下1行
    pop rcx
    loop .yihang

    add edi,32            ;下个字的行头
    sub edi,4*1024*16            ;上16行;现在edi=下个字开头

    mov eax,edi
    mov ebx,0x100000        ;ebx=vesabase
    ;mov ebx,[0x3028]        ;ebx=vesabase
    sub eax,ebx             ;eax=相对距离

.modfour:
    and eax,0x00000fff           ;mod4096
    cmp eax,0
    jne .nochange

.change:
    add edi,4*1024*16
    sub edi,1024*4

.nochange:
    pop rcx
    pop rbx

    ret              ;edi指向下一个字
;______________________________________
backcolor:dd 0
frontcolor:dd 0xffffffff




;____________________________________
writescreen:
    mov esi,0x100000
    mov edi,[0x3028]
    mov bl,[0x3019]
    shr bl,3
    movzx ebx,bl
    mov ecx,1024*768

.continue:
    lodsd
    mov [edi],eax
    add edi,ebx
    loop .continue

ret
;_________________________________
