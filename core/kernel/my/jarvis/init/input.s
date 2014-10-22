bits 64
%define kbdhome 0x800


;清空input buffer
    mov edi,kbdhome
    mov ecx,0x100
    xor rax,rax
    rep stosq

    mov rax,"input"
    mov [kbdhome+0x7e0],rax
    mov rax,kbdhome
    mov [kbdhome+0x7e8],rax
    mov rax,"progress"
    mov [kbdhome+0x7f0],rax
    mov rax,kbdhome
    mov [kbdhome+0x7f8],rax


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
	mov ebx,[kbdhome+0x7e8]			;地址
	mov [ebx],al
	inc ebx
	cmp ebx,kbdhome+0x7c0
	jb .newaddr
	mov ebx,kbdhome
	.newaddr:
	mov [kbdhome+0x7e8],ebx
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




sti
