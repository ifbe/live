bits 64
extern isr8
global enableidt28
global enableioapic8
global enableirq8


;_________________________________________
enableidt28:
	push rdi
	push rax

	mov rdi,0x1000+0x28*0x10
	lea rax,[rel wrap8]
	stosw
	mov dword [edi],0x8e000008
	add edi,4
	shr rax,16
	stosw
	shr rax,16
	stosd
	mov dword [edi],0

	pop rax
	pop rdi
	ret
;________________________________________




;________________________________________
enableioapic8:
	push rdi

	mov edi,0xfec00000
	mov dword [edi],0x10+	2*8
	mov dword [edi+0x10],0x28
	mov dword [edi],0x10+1+	2*8
	mov dword [edi+0x10],0

	pop rdi
	ret
;__________________________________________




;___________________________________________
enableirq8:
	push rax

	mov al,0x8a
	out 0x70,al
	mov al,0xac		;3=8192hz,6=1024hz,c=8hz,f=1hz
	out 0x71,al

	mov al,0x8b
	out 0x70,al
	in al,0x71
	mov ah,al
	or ah,0x40

	mov al,0x8b
	out 0x70,al
	mov al,ah
	out 0x71,al

	pop rax
	ret
;__________________________________




;_______________________________
wrap8:
;preserve everything
push rax
push rbx
push rcx
push rdx
push rsi
push rdi
push rbp

push r8
push r9
push r10
push r11
push r12
push r13
push r14
push r15

;must do
mov al,0x8c
out 0x70,al
in al,0x71

;real
call isr8

;eoi
mov rax,0xfee000b0
mov dword [rax],0

;restore everything
pop r15
pop r14
pop r13
pop r12
pop r11
pop r10
pop r9
pop r8

pop rbp
pop rdi
pop rsi
pop rdx
pop rcx
pop rbx
pop rax

iretq
;_______________________________

