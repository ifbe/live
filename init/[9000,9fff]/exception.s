[bits 64]
startofexception:

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
mov rax,exception20
call idtinstall
mov rax,exception21
call idtinstall
mov rax,exception22
call idtinstall
mov rax,exception23
call idtinstall
mov rax,exception24
call idtinstall
mov rax,exception25
call idtinstall
mov rax,exception26
call idtinstall
mov rax,exception27
call idtinstall
mov rax,exception28
call idtinstall
mov rax,exception29
call idtinstall
mov rax,exception2a
call idtinstall
mov rax,exception2b
call idtinstall
mov rax,exception2c
call idtinstall
mov rax,exception2d
call idtinstall
mov rax,exception2e
call idtinstall
mov rax,exception2f
call idtinstall
mov rax,exception30
call idtinstall
mov rax,exception31
call idtinstall
mov rax,exception32
call idtinstall
mov rax,exception33
call idtinstall
mov rax,exception34
call idtinstall
mov rax,exception35
call idtinstall
mov rax,exception36
call idtinstall
mov rax,exception37
call idtinstall
mov rax,exception38
call idtinstall
mov rax,exception39
call idtinstall
mov rax,exception3a
call idtinstall
mov rax,exception3b
call idtinstall
mov rax,exception3c
call idtinstall
mov rax,exception3d
call idtinstall
mov rax,exception3e
call idtinstall
mov rax,exception3f
call idtinstall

mov ecx,0xc0
external:
mov rax,unknown
call idtinstall
loop external

jmp endofexception
;_______________________________________


;_____________________________________
exception0:
mov byte [killer],0
jmp lastwords

exception1:
mov byte [killer],0x1
jmp lastwords

exception2:
mov byte [killer],0x2
jmp lastwords

exception3:
mov byte [killer],0x3
jmp lastwords

exception4:
mov byte [killer],0x4
jmp lastwords

exception5:
mov byte [killer],0x5
jmp lastwords

exception6:
mov byte [killer],0x6
jmp lastwords

exception7:
mov byte [killer],0x7
jmp lastwords

exception8:
pop qword [reason]
mov byte [killer],0x8
jmp lastwords

exception9:
mov byte [killer],0x9
jmp lastwords

exceptiona:
pop qword [reason]
mov byte [killer],0xa
jmp lastwords

exceptionb:
pop qword [reason]
mov byte [killer],0xb
jmp lastwords

exceptionc:
pop qword [reason]
mov byte [killer],0xc
jmp lastwords

exceptiond:
pop qword [reason]
mov byte [killer],0xd
jmp lastwords

exceptione:
pop qword [reason]
mov byte [killer],0xe
jmp lastwords

exceptionf:
mov byte [killer],0xf
jmp lastwords

exception10:
mov byte [killer],0x10
jmp lastwords

exception11:
pop qword [reason]
mov byte [killer],0x11
jmp lastwords

exception12:
mov byte [killer],0x12
jmp lastwords

exception13:
mov byte [killer],0x13
jmp lastwords

exception14:
mov byte [killer],0x14
jmp lastwords

exception15:
mov byte [killer],0x15
jmp lastwords

exception16:
mov byte [killer],0x16
jmp lastwords

exception17:
mov byte [killer],0x17
jmp lastwords

exception18:
mov byte [killer],0x18
jmp lastwords

exception19:
mov byte [killer],0x19
jmp lastwords

exception1a:
mov byte [killer],0x1a
jmp lastwords

exception1b:
mov byte [killer],0x1b
jmp lastwords

exception1c:
mov byte [killer],0x1c
jmp lastwords

exception1d:
mov byte [killer],0x1d
jmp lastwords

exception1e:
mov byte [killer],0x1e
jmp lastwords

exception1f:
mov byte [killer],0x1f
jmp lastwords

exception20:
mov byte [killer],0x20
jmp lastwords

exception21:
mov byte [killer],0x21
jmp lastwords

exception22:
mov byte [killer],0x22
jmp lastwords

exception23:
mov byte [killer],0x23
jmp lastwords

exception24:
mov byte [killer],0x24
jmp lastwords

exception25:
mov byte [killer],0x25
jmp lastwords

exception26:
mov byte [killer],0x26
jmp lastwords

exception27:
mov byte [killer],0x27
jmp lastwords

exception28:
mov byte [killer],0x28
jmp lastwords

exception29:
mov byte [killer],0x29
jmp lastwords

exception2a:
mov byte [killer],0x2a
jmp lastwords

exception2b:
mov byte [killer],0x2b
jmp lastwords

exception2c:
mov byte [killer],0x2c
jmp lastwords

exception2d:
mov byte [killer],0x2d
jmp lastwords

exception2e:
mov byte [killer],0x2e
jmp lastwords

exception2f:
mov byte [killer],0x2f
jmp lastwords

exception30:
mov byte [killer],0x30
jmp lastwords

exception31:
mov byte [killer],0x31
jmp lastwords

exception32:
mov byte [killer],0x32
jmp lastwords

exception33:
mov byte [killer],0x33
jmp lastwords

exception34:
mov byte [killer],0x34
jmp lastwords

exception35:
mov byte [killer],0x35
jmp lastwords

exception36:
mov byte [killer],0x36
jmp lastwords

exception37:
mov byte [killer],0x37
jmp lastwords

exception38:
mov byte [killer],0x38
jmp lastwords

exception39:
mov byte [killer],0x39
jmp lastwords

exception3a:
mov byte [killer],0x3a
jmp lastwords

exception3b:
mov byte [killer],0x3b
jmp lastwords

exception3c:
mov byte [killer],0x3c
jmp lastwords

exception3d:
mov byte [killer],0x3d
jmp lastwords

exception3e:
mov byte [killer],0x3e
jmp lastwords

exception3f:
mov byte [killer],0x3f
jmp lastwords

unknown:
mov dword [killer+4],0x01234567
mov dword [killer],0x89abcdef
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
mov [buffer+0x200],rax
mov rax,[rsp+8]
mov [buffer+0x210],rax
mov rax,[rsp+16]
mov [buffer+0x220],rax
mov rax,[rsp+24]
mov [buffer+0x230],rax
mov rax,[rsp+32]
mov [buffer+0x240],rax
mov rax,[rsp+40]
mov [buffer+0x250],rax
mov rax,[rsp+48]
mov [buffer+0x260],rax
mov rax,[rsp+56]
mov [buffer+0x270],rax


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
mov eax,[onepoint]
shl eax,8
lea eax,[eax*2+eax]
add edi,eax

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

cmp esi,endofbuffer
jb .continue
;________________________________________________


;_______________________________________
tobeornottobe:

;hlt              ;i am dead

in al,0x64
test al,1
jz tobeornottobe

in al,0x60
cmp al,0x9c		;enter
je leaveexception
cmp al,0x81		;esc
je turnoff

jmp tobeornottobe

turnoff:
mov dx,[0x4fc]
mov ax,[0x4fe]
or ax,0x2000
out dx,ax

leaveexception:
mov rax,[buffer]
mov rcx,[buffer+0x10]
mov rdx,[buffer+0x20]
mov rbx,[buffer+0x30]
mov rsp,[buffer+0x40]
mov rbp,[buffer+0x50]
mov rsi,[buffer+0x60]
mov rdi,[buffer+0x70]

mov r8,[buffer+0x80]
mov r9,[buffer+0x90]
mov r10,[buffer+0xa0]
mov r11,[buffer+0xb0]
mov r12,[buffer+0xc0]
mov r13,[buffer+0xd0]
mov r14,[buffer+0xe0]
mov r15,[buffer+0xf0]

iretq
;__________________________________


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

endofbuffer:

endofexception:
mov rax,0xc000
call rax
mov rax,0x10000
call rax

sleep:hlt
jmp sleep

padding:
times 0x1000-(padding-startofexception) db 0
