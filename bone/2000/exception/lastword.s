%define screeninfo 0x1000
[bits 64]


;________________________________________________
lastwords:

mov [rel buffer+8],rax
mov [rel buffer+0x18],rcx
mov [rel buffer+0x28],rdx
mov [rel buffer+0x38],rbx
mov [rel buffer+0x48],rsp
mov [rel buffer+0x58],rbp
mov [rel buffer+0x68],rsi
mov [rel buffer+0x78],rdi

mov [rel buffer+0x88],r8
mov [rel buffer+0x98],r9
mov [rel buffer+0xa8],r10
mov [rel buffer+0xb8],r11
mov [rel buffer+0xc8],r12
mov [rel buffer+0xd8],r13
mov [rel buffer+0xe8],r14
mov [rel buffer+0xf8],r15

mov rax,cr0
mov [rel buffer+0x108],rax
mov rax,cr2
mov [rel buffer+0x128],rax
mov rax,cr3
mov [rel buffer+0x138],rax
mov rax,cr4
mov [rel buffer+0x148],rax
mov rax,cr8
mov [rel buffer+0x158],rax

mov rax,dr0
mov [rel buffer+0x188],rax
mov rax,dr1
mov [rel buffer+0x198],rax
mov rax,dr2
mov [rel buffer+0x1a8],rax
mov rax,dr3
mov [rel buffer+0x1b8],rax
mov rax,dr6
mov [rel buffer+0x1e8],rax
mov rax,dr7
mov [rel buffer+0x1f8],rax

mov rax,[rsp]
mov [rel buffer+0x208],rax
mov rax,[rsp+8]
mov [rel buffer+0x218],rax
mov rax,[rsp+16]
mov [rel buffer+0x228],rax
mov rax,[rsp+24]
mov [rel buffer+0x238],rax
mov rax,[rsp+32]
mov [rel buffer+0x248],rax
mov rax,[rsp+40]
mov [rel buffer+0x258],rax
mov rax,[rsp+48]
mov [rel buffer+0x268],rax
mov rax,[rsp+56]
mov [rel buffer+0x278],rax


;________________varities____________________
mov al,[screeninfo+0x19]
movzx eax,al
shr eax,3
mov [rel onepoint],eax
shl eax,3
mov [rel onechar],eax
shl eax,7
mov [rel oneline],eax
shl eax,4
mov [rel sixteenline],eax

jmp theend
;______________________________________

onepoint:dq 0
onechar:dq 0
oneline:dq 0
sixteenline:dq 0




;________________the end,print to screen______________
theend:

mov edi,[screeninfo+0x28]
mov eax,[rel onepoint]
shl eax,8			;*256
lea eax,[eax*2+eax]		;*3
add edi,eax			;256*3=768也就是屏幕第一排3/4的位置
lea rsi,[rel buffer]		;项目放一块，开始地址在buffer

mov ecx,40
.explainone:
	push rsi
	push rdi
	call anscii
	pop rdi
	pop rsi

	push rsi
	push rdi
	call hex
	pop rdi
	pop rsi

	add esi,0x10			;buffer每16B一个类似"rax:0x1234"的记录
	add edi,[rel sixteenline]
loop .explainone




whatthen:
;hlt              ;i am dead
in al,0x64
test al,1
jz whatthen

in al,0x60
cmp al,0x1c		;enter
je leaveexception
cmp al,0x1		;esc
je turnoff

jmp whatthen
;___________________________________




;____________________________________
turnoff:
mov dx,[0x4fc]
mov ax,[0x4fe]
or ax,0x2000
out dx,ax
;_________________________________________




;_______________________________________
leaveexception:
mov rax,[rel buffer]
mov rcx,[rel buffer+0x18]
mov rdx,[rel buffer+0x28]
mov rbx,[rel buffer+0x38]
mov rsp,[rel buffer+0x48]
mov rbp,[rel buffer+0x58]
mov rsi,[rel buffer+0x68]
mov rdi,[rel buffer+0x78]

mov r8,[rel buffer+0x88]
mov r9,[rel buffer+0x98]
mov r10,[rel buffer+0xa8]
mov r11,[rel buffer+0xb8]
mov r12,[rel buffer+0xc8]
mov r13,[rel buffer+0xd8]
mov r14,[rel buffer+0xe8]
mov r15,[rel buffer+0xf8]

iretq
;__________________________________




;__________________________________________-
anscii:
    push rcx

    xor ecx,ecx
.print:
    mov al,[esi+ecx]
    cmp al,0x20
    jb .abnormal
    cmp al,0x80
    jae .abnormal
    jmp .normal				;[0x20,0x7f]是正常anscii

.abnormal:
    mov al,0x20

.normal:
    push rsi
    call char
    pop rsi

.next:
    inc ecx
    cmp ecx,8
    jb .print


    pop rcx
    ret
;____________________________________________




;____________________________________
hex:
push rcx

mov eax,[rel onechar]
shl eax,4
add edi,eax			;(在屏幕3/4处)再留出16个字

mov rdx,[esi+8]
mov ecx,16
.print:
    rol rdx,4
    mov al,dl
    and al,0xf

    push rdx
    call char
    pop rdx

loop .print    

pop rcx
ret
;______________________________________




;__________________________________________
;al字母，di显存位置
char:
    push rcx

    lea esi,[rel ansciitable]
    movzx eax,al
    shl eax,4
    add esi,eax

    mov ecx,16
.first:
    xor dx,dx
    mov dl,[esi]
    inc esi
    ;___________ 
.second:
    xor rax,rax
    not rax
    shl dl,1
    jc .skip
    mov eax,0xff
.skip:
    mov [edi],eax
    add edi,[rel onepoint]
    inc dh
    cmp dh,8
    jb .second
    ;___________

    sub edi,[rel onechar]
    add edi,[rel oneline]
    loop .first

sub edi,[rel sixteenline]
add edi,[rel onechar]

pop rcx
ret
;__________________________________________________




padding:
times 0x800-(padding-startofexception) db 0

endofexception:
