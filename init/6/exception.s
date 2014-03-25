[bits 64]
startofexception:

;__________________idt____________________
cli
mov word [0x600],0xfff
mov qword [0x602],0x1000
lidt [0x600]
;_____________________________________


;_______________null exception______________________
exceptioninstall:
mov edi,0x1000

call whereami	;push rip
whereami:
pop rbx
add rbx,exception0-whereami

mov rax,rbx		;now rax=exception0 handler
call idtinstall		;exception0
add rbx,12		;now rbx=exception1 handler

mov rax,rbx
call idtinstall		;exception1
add rbx,12

mov rax,rbx
call idtinstall		;exception2
add rbx,12

mov rax,rbx
call idtinstall		;exception3
add rbx,12

mov rax,rbx
call idtinstall		;exception4
add rbx,12

mov rax,rbx
call idtinstall		;exception5
add rbx,12

mov rax,rbx
call idtinstall		;exception6
add rbx,12

mov rax,rbx
call idtinstall		;exception7
add rbx,12

mov rax,rbx
call idtinstall		;exception8
add rbx,18		;!!!!

mov rax,rbx
call idtinstall		;exception9
add rbx,12

mov rax,rbx
call idtinstall		;exceptiona
add rbx,18		;!!!!

mov rax,rbx
call idtinstall		;exceptionb
add rbx,18		;!!!!

mov rax,rbx
call idtinstall		;exceptionc
add rbx,18		;!!!!

mov rax,rbx
call idtinstall		;exceptiond
add rbx,18		;!!!!

mov rax,rbx
call idtinstall		;exceptione
add rbx,18		;!!!!

mov rax,rbx
call idtinstall		;exceptionf
add rbx,12

mov rax,rbx
call idtinstall		;exception10
add rbx,12

mov rax,rbx
call idtinstall		;exception11
add rbx,18		;!!!!

mov rax,rbx
call idtinstall		;exception12
add rbx,12

mov rax,rbx
call idtinstall		;exception13
add rbx,12

mov rax,rbx
call idtinstall		;exception14
add rbx,12

mov rax,rbx
call idtinstall		;exception15
add rbx,12

mov rax,rbx
call idtinstall		;exception16
add rbx,12

mov rax,rbx
call idtinstall		;exception17
add rbx,12

mov rax,rbx
call idtinstall		;exception18
add rbx,12

mov rax,rbx
call idtinstall		;exception19
add rbx,12

mov rax,rbx
call idtinstall		;exception1a
add rbx,12

mov rax,rbx
call idtinstall		;exception1b
add rbx,12

mov rax,rbx
call idtinstall		;exception1c
add rbx,12

mov rax,rbx
call idtinstall		;exception1d
add rbx,12

mov rax,rbx
call idtinstall		;exception1e
add rbx,12

mov rax,rbx
call idtinstall		;exception1f
add rbx,12

mov ecx,0xe0
.external:
mov rax,rbx
call idtinstall
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
mov dword [rel killer+4],0x01234567
mov dword [rel killer],0x89abcdef
jmp lastwords
;________________________________________


;___________________________________
idtinstall:
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


;________________________________________________
lastwords:

mov [rel buffer],rax
mov [rel buffer+0x10],rcx
mov [rel buffer+0x20],rdx
mov [rel buffer+0x30],rbx
mov [rel buffer+0x40],rsp
mov [rel buffer+0x50],rbp
mov [rel buffer+0x60],rsi
mov [rel buffer+0x70],rdi

mov [rel buffer+0x80],r8
mov [rel buffer+0x90],r9
mov [rel buffer+0xa0],r10
mov [rel buffer+0xb0],r11
mov [rel buffer+0xc0],r12
mov [rel buffer+0xd0],r13
mov [rel buffer+0xe0],r14
mov [rel buffer+0xf0],r15

mov rax,cr0
mov [rel buffer+0x100],rax
mov rax,cr2
mov [rel buffer+0x120],rax
mov rax,cr3
mov [rel buffer+0x130],rax
mov rax,cr4
mov [rel buffer+0x140],rax
mov rax,cr8
mov [rel buffer+0x150],rax

mov rax,dr0
mov [rel buffer+0x180],rax
mov rax,dr1
mov [rel buffer+0x190],rax
mov rax,dr2
mov [rel buffer+0x1a0],rax
mov rax,dr3
mov [rel buffer+0x1b0],rax
mov rax,dr6
mov [rel buffer+0x1e0],rax
mov rax,dr7
mov [rel buffer+0x1f0],rax

mov rax,[rsp]
mov [rel buffer+0x200],rax
mov rax,[rsp+8]
mov [rel buffer+0x210],rax
mov rax,[rsp+16]
mov [rel buffer+0x220],rax
mov rax,[rsp+24]
mov [rel buffer+0x230],rax
mov rax,[rsp+32]
mov [rel buffer+0x240],rax
mov rax,[rsp+40]
mov [rel buffer+0x250],rax
mov rax,[rsp+48]
mov [rel buffer+0x260],rax
mov rax,[rsp+56]
mov [rel buffer+0x270],rax


;________________varities____________________
mov al,[0x3019]
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

mov edi,[0x3028]
mov eax,[rel onepoint]
shl eax,8
lea eax,[eax*2+eax]
add edi,eax

call whereami2
whereami2:
pop rsi
add rsi,buffer-whereami2

mov byte [rel howmany],0

	.continue:
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

	add esi,0x10
	add edi,[rel sixteenline]

inc byte [rel howmany]
cmp byte [rel howmany],40
jb .continue

whatthen:

;hlt              ;i am dead

in al,0x64
test al,1
jz whatthen

in al,0x60
cmp al,0x9c		;enter
je leaveexception
cmp al,0x81		;esc
je turnoff

jmp whatthen

turnoff:
mov dx,[0x4fc]
mov ax,[0x4fe]
or ax,0x2000
out dx,ax

leaveexception:
mov rax,[rel buffer]
mov rcx,[rel buffer+0x10]
mov rdx,[rel buffer+0x20]
mov rbx,[rel buffer+0x30]
mov rsp,[rel buffer+0x40]
mov rbp,[rel buffer+0x50]
mov rsi,[rel buffer+0x60]
mov rdi,[rel buffer+0x70]

mov r8,[rel buffer+0x80]
mov r9,[rel buffer+0x90]
mov r10,[rel buffer+0xa0]
mov r11,[rel buffer+0xb0]
mov r12,[rel buffer+0xc0]
mov r13,[rel buffer+0xd0]
mov r14,[rel buffer+0xe0]
mov r15,[rel buffer+0xf0]

iretq
;__________________________________
howmany: dq 0


;__________________________________________-
anscii:
    mov eax,[rel onechar]
    shl eax,3
    add edi,eax
    mov ecx,8
.print:
    push rsi
    mov al,[esi+ecx]
    cmp al,0x20
    jb .next
    cmp al,0x80
    jae .next
    movzx esi,al
    shl esi,4
    add esi,0xb000
    push rcx
    call char
    pop rcx
.next:
    pop rsi
    inc ecx
    cmp ecx,16
    jb .print
    ret
;____________________________________________


;____________________________________
hex:
    mov eax,[rel onechar]
    shl eax,4
    add edi,eax
    lodsq
    mov ecx,16
.print:
    rol rax,4
    mov dl,al
    shl dl,4
    push rax
    push rcx
    push rsi
    movzx esi,dl
    add esi,0xb000
    call char
    pop rsi
    pop rcx
    pop rax

    loop .print    
ret
;______________________________________


;__________________________________________
char:
    mov ecx,16
.first:
    xor dx,dx
    mov dl,[esi]
    inc esi
    ;___________ 
.second:
    xor rax,rax
    shl dl,1
    jnc .skip
    not rax
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
ret
;__________________________________________________


ALIGN 16
buffer:
dq 0,"rax"
dq 0,"rcx"
dq 0,"rdx"
dq 0,"rbx"
dq 0,"rsp"
dq 0,"rbp"
dq 0,"rsi"
dq 0,"rdi"

dq 0,"r8"
dq 0,"r9"
dq 0,"r10"
dq 0,"r11"
dq 0,"r12"
dq 0,"r13"
dq 0,"r14"
dq 0,"r15"

dq 0,"cr0"
dq 0,"i'm dead"
dq 0,"cr2"
dq 0,"cr3"
dq 0,"cr4"
dq 0,"cr8"
dq 0,"i'm dead"
dq 0,"i'm dead"

dq 0,"dr0"
dq 0,"dr1"
dq 0,"dr2"
dq 0,"dr3"
killer:
dq 0,"killer"
reason:
dq 0,"reason"
dq 0,"dr6"
dq 0,"dr7"

dq 0,"[rsp]"
dq 0,"[rsp+8]"
dq 0,"[rsp+16]"
dq 0,"[rsp+24]"
dq 0,"[rsp+32]"
dq 0,"[rsp+40]"
dq 0,"[rsp+48]"
dq 0,"[rsp+56]"

endofexception:
mov rax,0xc000
call rax
int3
mov rax,0x10000
call rax

sleep:hlt
jmp sleep

padding:
times 0x1000-(padding-startofexception) db 0
