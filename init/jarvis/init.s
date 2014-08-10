bits 64


cli


;_____________int 21h__________________
;keyboard:
lea rax,[rel keyboardisr]
mov edi,0x1210
call idtinstaller

;enable
mov edi,0xfec00000
mov dword [edi],0x10+2*0x1
mov dword [edi+0x10],0x21
mov dword [edi],0x10+2*0x1+1
mov dword [edi+0x10],0

jmp endofkeyboard


keyboardisr:
push rax
push rbx
	in al,0x60
	mov ebx,[0xfe8]			;地址
	mov [ebx],al
	inc ebx
	cmp ebx,0xfe0
	jb .newaddr
	mov ebx,0x800
	.newaddr:
	mov [0xfe8],ebx
.leave:
mov eax,0xfee000b0		;eoi
mov dword [eax],0
pop rbx
pop rax
iretq

endofkeyboard:
;________________________________________


jmp endofinterrupt

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
endofinterrupt:








;_______________________________________

;initahci
call endofjarvis		;ahci(0x1000)

;initxhci
call endofjarvis+0x2000		;xhci(0x1000)

;initdisk
call endofjarvis+0x4000		;disk(0x4000)
;__________________________________








;___________________________________________

    mov qword [rel addr],0x40000         ;r14 memory pointer
    mov qword [rel offset],0x420         ;r15 offset pointer

    mov edi,0x800
    mov ecx,0x100
    xor rax,rax
    rep stosq

    mov rax,"input"
    mov [0xfe0],rax
    mov rax,0x800
    mov [0xfe8],rax
    mov rax,"progress"
    mov [0xff0],rax
    mov rax,0x800
    mov [0xff8],rax

    lea rax,[rel function1]
    mov [rel screenwhat],rax
    lea rax,[rel menu]
    mov [rel mouseormenu],rax
    lea rax,[rel dumpanscii]
    mov [rel hexoranscii],rax

    sti
    xor rax,rax
    jmp decide
;____________________________________________________
