[bits 64]
startofexception:


;________________________________
cld
xor rax,rax

mov rcx,0x100
mov rdi,0x600
rep stosb

mov rcx,0x200
mov rdi,0x1000
rep stosq
;______________________________


;__________________idt____________________
cli
mov word [0x600],0xfff
mov qword [0x602],0x1000
lidt [0x600]
;_____________________________________


;_______________null exception______________________
exceptioninstall:
mov edi,0x1000

lea rax,[rel exception0]
call idtinstall

lea rax,[rel exception1]
call idtinstall		;exception1

lea rax,[rel exception2]
call idtinstall		;exception2

lea rax,[rel exception3]
call idtinstall		;exception3

lea rax,[rel exception4]
call idtinstall		;exception4

lea rax,[rel exception5]
call idtinstall		;exception5

lea rax,[rel exception6]
call idtinstall		;exception6

lea rax,[rel exception7]
call idtinstall		;exception7

lea rax,[rel exception8]
call idtinstall		;exception8

lea rax,[rel exception9]
call idtinstall		;exception9

lea rax,[rel exceptiona]
call idtinstall		;exceptiona

lea rax,[rel exceptionb]
call idtinstall		;exceptionb

lea rax,[rel exceptionc]
call idtinstall		;exceptionc

lea rax,[rel exceptiond]
call idtinstall		;exceptiond

lea rax,[rel exceptione]
call idtinstall		;exceptione

lea rax,[rel exceptionf]
call idtinstall		;exceptionf

lea rax,[rel exception10]
call idtinstall		;exception10

lea rax,[rel exception11]
call idtinstall		;exception11

lea rax,[rel exception12]
call idtinstall		;exception12

lea rax,[rel exception13]
call idtinstall		;exception13

lea rax,[rel exception14]
call idtinstall		;exception14

lea rax,[rel exception15]
call idtinstall		;exception15

lea rax,[rel exception16]
call idtinstall		;exception16

lea rax,[rel exception17]
call idtinstall		;exception17

lea rax,[rel exception18]
call idtinstall		;exception18

lea rax,[rel exception19]
call idtinstall		;exception19

lea rax,[rel exception1a]
call idtinstall		;exception1a

lea rax,[rel exception1b]
call idtinstall		;exception1b

lea rax,[rel exception1c]
call idtinstall		;exception1c

lea rax,[rel exception1d]
call idtinstall		;exception1d

lea rax,[rel exception1e]
call idtinstall		;exception1e

lea rax,[rel exception1f]
call idtinstall		;exception1f

mov ecx,0xe0
.external:
lea rax,[rel unknown]
call idtinstall
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
mov dword [rel killer+4],0x01234567
mov dword [rel killer],0x89abcdef
push rax
mov rax,0xfee000b0
mov dword [rax],0
pop rax
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
