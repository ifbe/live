[bits 64]




;_______________prepare 8254______________
	cli								;Disabled interrupts (just in case)

	mov al,00110100b				;channel 0, lobyte/hibyte, rate generator
	out 0x43, al

	mov ax,1192						;1.192182mhz
	out 0x40,al						;Set low byte of PIT reload value
	mov al,ah						;ax = high 8 bits of reload value
	out 0x40,al						;Set high byte of PIT reload value
;_____________________________________________




;_____________222222222222222__________________
;isr32
	mov r8,0x20
	lea r9,[rel isr32]
	call isrinstall

;enable
	mov edi,0xfec00000
	mov dword [edi],0x10
	mov dword [edi+0x10],0x20
	mov dword [edi],0x10+1
	mov dword [edi+0x10],0

	jmp endof8254
;________________________________________




;______________________________________
isr32:
	push rax
	mov eax,0xfee000b0				;eoi
	mov dword [eax],0
	pop rax

	iretq
;________________________________________




endof8254:
sti
