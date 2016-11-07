[bits 64]




;_____________________________________________
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




whatthen:
	;hlt              ;i am dead
	in al,0x64
	test al,1
	jz whatthen

	in al,0x60
	cmp al,0x1		;esc
	je turnoff

	cmp al,0x1c		;enter
	jne whatthen




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
