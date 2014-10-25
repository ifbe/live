[bits 64]


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

lea rsi,[rel buffer]
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
cmp al,0x1c		;enter
je leaveexception
cmp al,0x01		;esc
je turnoff

jmp whatthen
;___________________________________
howmany: dq 0




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
    movzx eax,al
    ;mov esi,eax
    ;shl esi,4
    ;add esi,0x5000
    lea esi,[rel ansciitable]
    lea esi,[esi+eax*4]
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
    push rax
    push rcx
    push rsi
    ;mov esi,edx
    ;add esi,0x5000
    movzx edx,dl
    lea esi,[rel ansciitable]
    lea esi,[esi+edx*4]
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




padding:
times 0x800-(padding-startofexception) db 0

endofexception:
