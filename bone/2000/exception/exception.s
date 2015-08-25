%define idtr 0x830
%define idthome 0x3000

[bits 64]
startofexceptionhandler:




;_______________________________________
	cld
	cli
;清空interrupt descriptor table
	mov rdi,idthome
	xor rax,rax
	mov rcx,0x200
	rep stosq

;写入table的大小和位置
	mov rdi,idtr
	mov ax,0xfff
	stosw			;[+0,+1]
	mov rax,idthome
	stosq			;[+2,+9]
	xor rax,rax
	stosw			;[+0xa,+0xb]
	stosd			;[+0xc,+0xf]

	lidt [idtr]
;_____________________________________


;_______________null exception______________________
exceptioninstall:
	mov edi,idthome

	lea rax,[rel exception0]
	call isrinstall

	lea rax,[rel exception1]
	call isrinstall		;exception1

	lea rax,[rel exception2]
	call isrinstall		;exception2

	lea rax,[rel exception3]
	call isrinstall		;exception3

	lea rax,[rel exception4]
	call isrinstall		;exception4

	lea rax,[rel exception5]
	call isrinstall		;exception5

	lea rax,[rel exception6]
	call isrinstall		;exception6

	lea rax,[rel exception7]
	call isrinstall		;exception7

	lea rax,[rel exception8]
	call isrinstall		;exception8

	lea rax,[rel exception9]
	call isrinstall		;exception9

	lea rax,[rel exceptiona]
	call isrinstall		;exceptiona

	lea rax,[rel exceptionb]
	call isrinstall		;exceptionb

	lea rax,[rel exceptionc]
	call isrinstall		;exceptionc

	lea rax,[rel exceptiond]
	call isrinstall		;exceptiond

	lea rax,[rel exceptione]
	call isrinstall		;exceptione

	lea rax,[rel exceptionf]
	call isrinstall		;exceptionf

	lea rax,[rel exception10]
	call isrinstall		;exception10

	lea rax,[rel exception11]
	call isrinstall		;exception11

	lea rax,[rel exception12]
	call isrinstall		;exception12

	lea rax,[rel exception13]
	call isrinstall		;exception13

	lea rax,[rel exception14]
	call isrinstall		;exception14

	lea rax,[rel exception15]
	call isrinstall		;exception15

	lea rax,[rel exception16]
	call isrinstall		;exception16

	lea rax,[rel exception17]
	call isrinstall		;exception17

	lea rax,[rel exception18]
	call isrinstall		;exception18

	lea rax,[rel exception19]
	call isrinstall		;exception19

	lea rax,[rel exception1a]
	call isrinstall		;exception1a

	lea rax,[rel exception1b]
	call isrinstall		;exception1b

	lea rax,[rel exception1c]
	call isrinstall		;exception1c

	lea rax,[rel exception1d]
	call isrinstall		;exception1d

	lea rax,[rel exception1e]
	call isrinstall		;exception1e

	lea rax,[rel exception1f]
	call isrinstall		;exception1f

	mov ecx,0xe0
.external:
	lea rax,[rel unknown]
	call isrinstall
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
mov qword [rel killer],0xffffffffffffffff
push rax
mov rax,0xfee000b0
mov dword [rax],0
pop rax
jmp lastwords
;________________________________________




;___________________________________
isrinstall:
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
