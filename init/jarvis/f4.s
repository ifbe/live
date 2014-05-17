bits 64
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>----console----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;_______________________________________________
function4:
cmp al,0
je console		;do nothing
cmp al,0x3b
je f1
cmp al,0x3c
je f2
cmp al,0x3d
je f3
cmp al,0x3e
je f4
cmp al,0x1c
je f4enter
cmp al,0x0e
je f4backspace

jmp f4other
;_______________________________________________________________________________


;_______________________________
f4backspace:

cmp byte [rel length],8
jna console

lea ebx,[rel line0]
add ebx,[rel linex128]
dec byte [rel length]
add ebx,[rel length]
mov byte [ebx],' '

jmp console
;_________________________________________


;_____________________________________
f4other:

cmp byte [rel length],128
jae console

call scan2anscii

record:
lea ebx,[rel line0]
add ebx,[rel linex128]
add ebx,[rel length]
mov [ebx],al
inc byte [rel length]
jmp console
;___________________________________


;___________________________________
f4enter:

lea eax,[rel line0]
add eax,[rel linex128]
add eax,8
mov [rel currentarg],eax		;esi=current argument

mov esi,[rel currentarg]
cmp dword [esi],"powe"		;if [esi]=="powe"
jne skipturnoff
cmp dword [esi+4],"roff"	;if [esi+4]=="roff"
jne skipturnoff
cmp byte [esi+8],' '		;if [esi+8]=0x20
je turnoff
skipturnoff:

mov esi,[rel currentarg]
cmp dword [esi],"clea"
jne skipclear
cmp dword [esi+4],"r   "
je clear
skipclear:

mov esi,[rel currentarg]
cmp dword [esi],"addr"		;if [esi]="addr"
jne skipaddr
cmp byte [esi+4],'='		;if [esi+4]="="
je changeaddr
skipaddr:

mov esi,[rel currentarg]
cmp dword [esi],"test"
jne skiptest
cmp byte [esi+4],0x20
je test
skiptest:

mov esi,[rel currentarg]
cmp dword [esi],"call"
jne skipcast
cmp byte [esi+4],0x20
je cast
skipcast:

mov esi,[rel currentarg]
cmp dword [esi],"jump"
jne skipjump
cmp byte [esi+4],0x20
je jump
skipjump:

mov esi,[rel currentarg]
cmp word [esi],"ls"
jne skipls
cmp byte [esi+2],0x20
je ls
skipls:

mov esi,[rel currentarg]
cmp word [esi],"cd"
jne skipcd
cmp byte [esi+2],0x20
je cd
skipcd:

mov esi,[rel currentarg]
cmp dword [esi],"load"
jne skipload
cmp byte [esi+4],0x20
je load
skipload:

notfound:
	call checkandchangeline
	lea edi,[rel line0]
	add edi,[rel linex128]
	mov dword [edi],"notf"
	mov dword [edi+4],"ound"

scroll:
	call checkandchangeline
	lea edi,[rel line0]
	add edi,[rel linex128]
	mov dword [edi],"[   "
	mov dword [edi+4],"/]$ "
	mov dword [rel length],8
	jmp console
;_________________________________
currentarg:dq 0		;only this function use me




;_________________________________
checkandchangeline:
	cmp dword [rel linex128],0x80*47	;current=last?
	jae .move

	add dword [rel linex128],128		;no:line+1
	jmp .return

	.move:					;yes:move
	lea esi,[rel line1]
	lea edi,[rel line0]
	mov ecx,128*0x30
	cld
	rep movsb

	.return:
	ret					;now line=a blank line
;____________________________________




;____________________________________
clear:
lea edi,[rel line0]
mov al,0x20
mov ecx,128*0x30
rep stosb
lea edi,[rel line0]
mov rax,"[   /]$ "
stosq
mov dword [rel linex128],0
mov dword [rel length],8
jmp console
;_______________________________________




;_________________________
ls:

call checkandchangeline
lea edi,[rel line0]
add edi,[rel linex128]
mov esi,0x80000

mov ecx,8
.many:
movsq
mov byte [edi],'.'
inc edi
movsw
movsb
add esi,0x15
add edi,0x3
loop .many

jmp scroll
;_________________________




;________________________
cd:
cmp word [0x8000],"cd"
jne notfound
mov rax,[rsi+3]

mov ecx,8
.fuckanscii:
rol rax,8
cmp al,0x60
jb .next
sub al,0x20
.next:
loop .fuckanscii

mov rdi,rax
call [0x8008]
jmp scroll
;______________________




;________________________
load:
cmp dword [0x8010],"load"
jne notfound
mov rax,[rsi+5]

mov ecx,8
.fuckanscii:
rol rax,8
cmp al,0x60
jb .next
sub al,0x20
.next:
loop .fuckanscii

mov rdi,rax
call [0x8018]
jmp scroll
;______________________




;______________________
changeaddr:

add esi,5
call fetchvalue
mov rax,[rel fetched]
mov [rel addr],rax
jmp scroll
;_________________________


;______________________________
test:
mov rax,0x100000
call rax
jmp scroll
;______________________________


;_______________________________
cast:

add esi,5
call fetchvalue
call [rel fetched]
jmp scroll
;_______________________________


;_______________________________
jump:

add esi,5
call fetchvalue
jmp [rel fetched]
;_______________________________


;________________________________
fetchvalue:
mov qword [rel fetched],0
mov ecx,16
.part:

lodsb                ;get one char

cmp al,0x30          ;<0x30,error
jb notfound
cmp al,0x39          ;>0x39,maybe a~f
ja .atof
sub al,0x30          ;now its certainly 0~9
jmp .generate

.atof:
cmp al,0x61          ;[0x40,0x60],error
jb notfound
cmp al,0x66          ;>0x66,error
ja notfound
sub al,0x57          ;now its certainly a~f

.generate:
add byte [rel fetched],al
cmp byte [esi],0x20
je .finish
shl qword [rel fetched],4

loop .part

.finish:
ret
;_______________________________-
fetched:dq 0



;___________________________________________
console:
mov edi,0x1000000
mov ecx,1024*750
xor eax,eax
rep stosd

mov dword [rel looptimes],0
.lines:
    mov edi,0x1000000
    mov eax,4*1024*16
    ;add edi,eax
    imul eax,[rel looptimes]
    add edi,eax
    lea ebx,[rel line0]
    mov eax,[rel looptimes]
    shl eax,7
    add ebx,eax

    mov cl,128
    .continue:
    mov al,[ebx]
    call char
    inc ebx
    dec cl
    jnz .continue

inc byte [rel looptimes]
cmp byte [rel looptimes],0x30
jb .lines

call writescreen
jmp forever
;________________________________________
looptimes:dd 0








linex128:dq 0
length:dq 8
line0:		db "[   /]$ "
		times 120 db ' '
line1:times 0x30*128 db ' '
