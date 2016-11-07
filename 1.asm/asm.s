%define idtr 0x850
%define idthome 0x5000

[bits 64]
startofexception:




;_______________________________________
	cld
	cli
;清空interrupt descriptor table
	mov rdi,idthome
	xor rax,rax
	mov rcx,0x200
	rep stosq

;写入table的大小和位置
	mov rdi,idtr
	mov ax,0xfff
	stosw			;[+0,+1]
	mov rax,idthome
	stosq			;[+2,+9]
	xor rax,rax
	stosw			;[+0xa,+0xb]
	stosd			;[+0xc,+0xf]

	lidt [idtr]
;_____________________________________


;_______________null exception______________________
exceptioninstall:
	mov edi,idthome

	lea rax,[rel exception0]
	call isrinstall

	lea rax,[rel exception1]
	call isrinstall		;exception1

	lea rax,[rel exception2]
	call isrinstall		;exception2

	lea rax,[rel exception3]
	call isrinstall		;exception3

	lea rax,[rel exception4]
	call isrinstall		;exception4

	lea rax,[rel exception5]
	call isrinstall		;exception5

	lea rax,[rel exception6]
	call isrinstall		;exception6

	lea rax,[rel exception7]
	call isrinstall		;exception7

	lea rax,[rel exception8]
	call isrinstall		;exception8

	lea rax,[rel exception9]
	call isrinstall		;exception9

	lea rax,[rel exceptiona]
	call isrinstall		;exceptiona

	lea rax,[rel exceptionb]
	call isrinstall		;exceptionb

	lea rax,[rel exceptionc]
	call isrinstall		;exceptionc

	lea rax,[rel exceptiond]
	call isrinstall		;exceptiond

	lea rax,[rel exceptione]
	call isrinstall		;exceptione

	lea rax,[rel exceptionf]
	call isrinstall		;exceptionf

	lea rax,[rel exception10]
	call isrinstall		;exception10

	lea rax,[rel exception11]
	call isrinstall		;exception11

	lea rax,[rel exception12]
	call isrinstall		;exception12

	lea rax,[rel exception13]
	call isrinstall		;exception13

	lea rax,[rel exception14]
	call isrinstall		;exception14

	lea rax,[rel exception15]
	call isrinstall		;exception15

	lea rax,[rel exception16]
	call isrinstall		;exception16

	lea rax,[rel exception17]
	call isrinstall		;exception17

	lea rax,[rel exception18]
	call isrinstall		;exception18

	lea rax,[rel exception19]
	call isrinstall		;exception19

	lea rax,[rel exception1a]
	call isrinstall		;exception1a

	lea rax,[rel exception1b]
	call isrinstall		;exception1b

	lea rax,[rel exception1c]
	call isrinstall		;exception1c

	lea rax,[rel exception1d]
	call isrinstall		;exception1d

	lea rax,[rel exception1e]
	call isrinstall		;exception1e

	lea rax,[rel exception1f]
	call isrinstall		;exception1f

	mov ecx,0xe0
.external:
	lea rax,[rel unknown]
	call isrinstall
	loop .external

	jmp endofexception
;_______________________________________




;_____________________________________
exception0:
mov byte [rel killer],0
jmp lastwords

exception1:
mov byte [rel killer],0x1
jmp lastwords

exception2:
mov byte [rel killer],0x2
jmp lastwords

exception3:
mov byte [rel killer],0x3
jmp lastwords

exception4:
mov byte [rel killer],0x4
jmp lastwords

exception5:
mov byte [rel killer],0x5
jmp lastwords

exception6:
mov byte [rel killer],0x6
jmp lastwords

exception7:
mov byte [rel killer],0x7
jmp lastwords

exception8:
pop qword [rel reason]
mov byte [rel killer],0x8
jmp lastwords

exception9:
mov byte [rel killer],0x9
jmp lastwords

exceptiona:
pop qword [rel reason]
mov byte [rel killer],0xa
jmp lastwords

exceptionb:
pop qword [rel reason]
mov byte [rel killer],0xb
jmp lastwords

exceptionc:
pop qword [rel reason]
mov byte [rel killer],0xc
jmp lastwords

exceptiond:
pop qword [rel reason]
mov byte [rel killer],0xd
jmp lastwords

exceptione:
pop qword [rel reason]
mov byte [rel killer],0xe
jmp lastwords

exceptionf:
mov byte [rel killer],0xf
jmp lastwords

exception10:
mov byte [rel killer],0x10
jmp lastwords

exception11:
pop qword [rel reason]
mov byte [rel killer],0x11
jmp lastwords

exception12:
mov byte [rel killer],0x12
jmp lastwords

exception13:
mov byte [rel killer],0x13
jmp lastwords

exception14:
mov byte [rel killer],0x14
jmp lastwords

exception15:
mov byte [rel killer],0x15
jmp lastwords

exception16:
mov byte [rel killer],0x16
jmp lastwords

exception17:
mov byte [rel killer],0x17
jmp lastwords

exception18:
mov byte [rel killer],0x18
jmp lastwords

exception19:
mov byte [rel killer],0x19
jmp lastwords

exception1a:
mov byte [rel killer],0x1a
jmp lastwords

exception1b:
mov byte [rel killer],0x1b
jmp lastwords

exception1c:
mov byte [rel killer],0x1c
jmp lastwords

exception1d:
mov byte [rel killer],0x1d
jmp lastwords

exception1e:
mov byte [rel killer],0x1e
jmp lastwords

exception1f:
mov byte [rel killer],0x1f
jmp lastwords

unknown:
mov qword [rel killer],0xffffffffffffffff
push rax
mov rax,0xfee000b0
mov dword [rax],0
pop rax
jmp lastwords
;________________________________________




;___________________________________
isrinstall:
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




%define screeninfo 0x2000
%define ansciitable 0x3000

%define thatport 0xffc
%define thatdata 0xffe
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
mov dx,[thatport]
mov ax,[thatdata]
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

    ;lea esi,[rel ansciitable]
    mov esi,ansciitable
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




;_______________________________________________
align 16
buffer:
dq "rax",0
dq "rcx",0
dq "rdx",0
dq "rbx",0
dq "rsp",0
dq "rbp",0
dq "rsi",0
dq "rdi",0

dq "r8",0
dq "r9",0
dq "r10",0
dq "r11",0
dq "r12",0
dq "r13",0
dq "r14",0
dq "r15",0

dq "cr0",0
dq "i'm dead",0
dq "cr2",0
dq "cr3",0
dq "cr4",0
dq "cr8",0
dq "i'm dead",0
dq "i'm dead",0

dq "dr0",0
dq "dr1",0
dq "dr2",0
dq "dr3",0
dq "killer"
killer:dq 0
dq "reason"
reason:dq 0
dq "dr6",0
dq "dr7",0

dq "[rsp]",0
dq "[rsp+8]",0
dq "[rsp+16]",0
dq "[rsp+24]",0
dq "[rsp+32]",0
dq "[rsp+40]",0
dq "[rsp+48]",0
dq "[rsp+56]",0
;________________________________________________




;_____________________________________________
padding:
times 0x1000-(padding-startofexception) db 0

endofexception:
