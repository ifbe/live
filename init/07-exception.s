[bits 64]
;__________________idt____________________
cli
mov word [idtptr.length],0xfff
mov qword [idtptr.base],0x1000
lidt [idtptr]
jmp exceptioninstall

ALIGN 16
idtptr:
.length dw 0
.base dq 0
;_____________________________________


;_______________null exception______________________
exceptioninstall:
mov edi,0x1000

mov rax,exception0
call idtinstall
mov rax,exception1
call idtinstall
mov rax,exception2
call idtinstall
mov rax,exception3
call idtinstall
mov rax,exception4
call idtinstall
mov rax,exception5
call idtinstall
mov rax,exception6
call idtinstall
mov rax,exception7
call idtinstall
mov rax,exception8
call idtinstall
mov rax,exception9
call idtinstall
mov rax,exceptiona
call idtinstall
mov rax,exceptionb
call idtinstall
mov rax,exceptionc
call idtinstall
mov rax,exceptiond
call idtinstall
mov rax,exceptione
call idtinstall
mov rax,exceptionf
call idtinstall
mov rax,exception10
call idtinstall
mov rax,exception11
call idtinstall
mov rax,exception12
call idtinstall
mov rax,exception13
call idtinstall
mov rax,exception14
call idtinstall
mov rax,exception15
call idtinstall
mov rax,exception16
call idtinstall
mov rax,exception17
call idtinstall
mov rax,exception18
call idtinstall
mov rax,exception19
call idtinstall
mov rax,exception1a
call idtinstall
mov rax,exception1b
call idtinstall
mov rax,exception1c
call idtinstall
mov rax,exception1d
call idtinstall
mov rax,exception1e
call idtinstall
mov rax,exception1f
call idtinstall

mov ecx,0xe0
external:
mov rax,lastwords
call idtinstall
loop external

jmp exceptiondone
;_______________________________________


;_____________________________________
exception0:
push 0
jmp lastwords

exception1:
push 1
jmp lastwords

exception2:
push 2
jmp lastwords

exception3:
push 3
jmp lastwords

exception4:
push 4
jmp lastwords

exception5:
push 5
jmp lastwords

exception6:
push 6
jmp lastwords

exception7:
push 7
jmp lastwords

exception8:
push 8
jmp lastwords

exception9:
push 9
jmp lastwords

exceptiona:
push 0xa
jmp lastwords

exceptionb:
push 0xb
jmp lastwords

exceptionc:
push 0xc
jmp lastwords

exceptiond:
push 0xd
jmp lastwords

exceptione:
push 0xe
jmp lastwords

exceptionf:
push 0xf
jmp lastwords

exception10:
push 0x10
jmp lastwords

exception11:
push 0x11
jmp lastwords

exception12:
push 0x12
jmp lastwords

exception13:
push 0x13
jmp lastwords

exception14:
push 0x14
jmp lastwords

exception15:
push 0x15
jmp lastwords

exception16:
push 0x16
jmp lastwords

exception17:
push 0x17
jmp lastwords

exception18:
push 0x18
jmp lastwords

exception19:
push 0x19
jmp lastwords

exception1a:
push 0x1a
jmp lastwords

exception1b:
push 0x1b
jmp lastwords

exception1c:
push 0x1c
jmp lastwords

exception1d:
push 0x1d
jmp lastwords

exception1e:
push 0x1e
jmp lastwords

exception1f:
push 0x1f
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

;____________________fill buffer____________________
mov [buffer],rax
mov [buffer+0x10],rcx
mov [buffer+0x20],rdx
mov [buffer+0x30],rbx
mov [buffer+0x40],rsp
mov [buffer+0x50],rbp
mov [buffer+0x60],rsi
mov [buffer+0x70],rdi

mov [buffer+0x80],r8
mov [buffer+0x90],r9
mov [buffer+0xa0],r10
mov [buffer+0xb0],r11
mov [buffer+0xc0],r12
mov [buffer+0xd0],r13
mov [buffer+0xe0],r14
mov [buffer+0xf0],r15

mov rax,cr0
mov [buffer+0x100],rax
mov rax,cr2
mov [buffer+0x120],rax
mov rax,cr3
mov [buffer+0x130],rax
mov rax,cr4
mov [buffer+0x140],rax
mov rax,cr8
mov [buffer+0x150],rax

mov rax,dr0
mov [buffer+0x180],rax
mov rax,dr1
mov [buffer+0x190],rax
mov rax,dr2
mov [buffer+0x1a0],rax
mov rax,dr3
mov [buffer+0x1b0],rax
mov rax,dr6
mov [buffer+0x1e0],rax
mov rax,dr7
mov [buffer+0x1f0],rax

mov rax,[rsp]
mov [buffer+0x1f0],rax
mov rax,[rsp+8]
mov [buffer+0x1f0],rax
mov rax,[rsp+16]
mov [buffer+0x1f0],rax
mov rax,[rsp+24]
mov [buffer+0x1f0],rax
mov rax,[rsp+32]
mov [buffer+0x1f0],rax
mov rax,[rsp+40]
mov [buffer+0x1f0],rax

pop rax
mov [killer],rax
;_______________________________________


;________________varities____________________
mov al,[0x3019]
movzx eax,al
shr eax,3
mov [onepoint],eax
shl eax,3
mov [onechar],eax
shl eax,7
mov [oneline],eax
shl eax,4
mov [sixteenline],eax

jmp theend
;______________________________________

onepoint:dq 0
onechar:dq 0
oneline:dq 0
sixteenline:dq 0


;________________the end,print to screen______________
theend:
mov esi,buffer
mov edi,[0x3028]

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
	add edi,[sixteenline]

cmp esi,exceptiondone
jb .continue

hlt              ;i am dead
;________________________________________________


;__________________________________________-
anscii:
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
    mov eax,[onechar]
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
    add edi,[onepoint]
    inc dh
    cmp dh,8
    jb .second
    ;___________

    sub edi,[onechar]
    add edi,[oneline]
    loop .first

sub edi,[sixteenline]
add edi,[onechar]
ret
;__________________________________________________


ALIGN 16
buffer:
dq 0,"rax="
dq 0,"rcx="
dq 0,"rdx="
dq 0,"rbx="
dq 0,"rsp="
dq 0,"rbp="
dq 0,"rsi="
dq 0,"rdi="

dq 0,"r8 ="
dq 0,"r9 ="
dq 0,"r10="
dq 0,"r11="
dq 0,"r12="
dq 0,"r13="
dq 0,"r14="
dq 0,"r15="

dq 0,"cr0="
dq 0,0
dq 0,"cr2="
dq 0,"cr3="
dq 0,"cr4="
dq 0,"cr8="
dq 0,0
dq 0,0

dq 0,"dr0="
dq 0,"dr1="
dq 0,"dr2="
dq 0,"dr3="
dq 0,0
dq 0,0
dq 0,"dr6="
dq 0,"dr7="

dq 0,"stack+0"
dq 0,"stack+8"
dq 0,"stack+16"
dq 0,"stack+24"
dq 0,"stack+32"
dq 0,"stack+40"
dq 0,"stack+48"
dq 0,"stack+56"

killer:
dq 0,"inherent"
dq 0,"in"
dq 0,"a"
dq 0,"death"
dq 0,"but....."
dq 0,"the....."
dq 0,"killer.."
dq 0,"is......"

exceptiondone:
