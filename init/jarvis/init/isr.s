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
	cmp ebx,0xfc0
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




sti
