bits 64
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>----console----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;取一只微小的蚂蚁，让它影响世界
;这是最后一只?
;是的：拍一张新世界的照片到屏幕上，而我回家睡觉
;不是：再再来一次



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
cmp al,0x3f
je f5
cmp al,0x1c
je f4enter
cmp al,0x0e
je f4backspace

cmp al,0x80
ja console
jmp f4other
;_______________________________________________________________________________


;_______________________________
f4backspace:

cmp byte [rel length],8
jna console

;lea ebx,[rel line0]
mov ebx,0x6000
add ebx,[rel linex128]
dec byte [rel length]
add ebx,[rel length]
mov byte [ebx],0

jmp console
;_________________________________________


;_____________________________________
f4other:

cmp byte [rel length],128
jae console

call scan2anscii

record:
;lea ebx,[rel line0]
mov ebx,0x6000
add ebx,[rel linex128]
add ebx,[rel length]
mov [ebx],al
inc byte [rel length]
jmp console
;___________________________________


;___________________________________
f4enter:

;lea eax,[rel line0]
mov eax,0x6000
add eax,[rel linex128]
add eax,8
mov [rel currentarg],eax		;esi=current argument

mov esi,[rel currentarg]
cmp dword [esi],"powe"		;if [esi]=="powe"
jne skipturnoff
cmp dword [esi+4],"roff"	;if [esi+4]=="roff"
jne skipturnoff
cmp byte [esi+8],0		;if [esi+8]=0x20
je turnoff
skipturnoff:

mov esi,[rel currentarg]
cmp dword [esi],"exit"
jne skipexit
cmp byte [esi+4],0
je turnoff
skipexit:


mov esi,[rel currentarg]
cmp dword [esi],"clea"
jne skipclear
cmp byte [esi+4],'r'
je clear
skipclear:

mov esi,[rel currentarg]
cmp dword [esi],"test"
je test

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
je ls

searchprogram:
	mov esi,[rel currentarg]
	call fetchanscii
	call cmdtoaddr

	mov esi,[rel currentarg]
	.continue:
	cmp byte [esi],0x20
	jna .fetch
	inc esi
	jmp .continue
	.fetch:
	inc esi
	call fetchanscii

	call checkandchangeline
	mov rbx,[rel address]
	call explainrbx
	mov rbx,[rel anscii]
	call explainrbx

	mov rax,[rel address]
	cmp rax,0
	je scroll
	mov rdi,[rel anscii]
	call rax
	jmp scroll

notfound:
	call checkandchangeline
	mov dword [edi],"notf"
	mov dword [edi+4],"ound"

scroll:
	call checkandchangeline
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
	push rsi
	push rcx
	;lea esi,[rel line1]
	mov esi,0x6080
	;lea edi,[rel line0]
	mov edi,0x6000
	mov ecx,128*0x30
	cld
	rep movsb
	pop rcx
	pop rsi

	.return:
	;lea edi,[rel line0]
	mov edi,0x6000
	add edi,[rel linex128]
	ret					;now line=a blank line
;____________________________________




;_________________________________
explainrbx:		;care about 32bit only,because......
	add edi,8
	rol rbx,32

	mov ecx,8
	.continue2:
	rol rbx,4
	mov al,bl
	and al,0xf
	add al,0x30
	cmp al,0x3a
	jb .done
	add al,7
	.done:
	stosb
	loop .continue2

	ret
;_______________________________




;____________________________________
clear:
;lea edi,[rel line0]
mov edi,0x6000
mov ecx,128*0x30
xor rax,rax
rep stosb
;lea edi,[rel line0]
mov edi,0x6000
mov rax,"[   /]$ "
stosq
mov dword [rel linex128],0
mov dword [rel length],8
jmp console
;_______________________________________




;_________________________
ls:

call checkandchangeline		;get new edi
mov esi,0x1c0000
xor ecx,ecx
.continue:
cmp dword [esi],0
je scroll
movsq
add esi,0x18
add edi,0x8
inc ecx

cmp ecx,0x200
jae scroll
test ecx,0x7
jnz .continue
call checkandchangeline
jmp .continue
;_________________________




;______________________________
test:
mov rax,0x400000
call rax
jmp scroll
;______________________________


;_______________________________
cast:

add esi,5
call fetchvalue
call [rel value]
jmp scroll
;_______________________________


;_______________________________
jump:

add esi,5
call fetchvalue
jmp [rel value]
;_______________________________


;______________________________
fetchanscii:
mov qword [rel anscii],0

lea rdi,[rel anscii]
mov ecx,8
.continue:
lodsb
cmp al,0x20
jbe .return
cmp al,0x7a
ja .return
stosb
loop .continue

.return:
ret
;__________________________________
anscii:dq 0





;____________________________________
cmdtoaddr:
mov qword [rel address],0

mov rax,[rel anscii]
mov esi,0x4000
.continue2:
cmp esi,0x5000
jae .return
cmp [esi],rax
je .find
add esi,0x10
jmp .continue2

.find:
mov rax,[esi+8]
mov [rel address],rax

.return:
ret
;__________________________________________
address:dq 0


;________________________________
fetchvalue:
mov qword [rel value],0
mov ecx,16
.part:

lodsb                ;get one char

cmp al,0x30
jb .finish
cmp al,0x39          ;>0x39,maybe a~f
ja .atof
sub al,0x30          ;now its certainly 0~9
jmp .generate

.atof:
cmp al,0x61          ;[0x40,0x60],error
jb .finish
cmp al,0x66          ;>0x66,error
ja .finish
sub al,0x57          ;now its certainly a~f

.generate:
shl qword [rel value],4
add byte [rel value],al
loop .part

.finish:
ret
;_______________________________-
value:dq 0



;___________________________________________
console:
mov edi,0x1c00000
mov ecx,1024*750
xor eax,eax
rep stosd


mov dword [rel looptimes],0
.lines:
    mov edi,0x1c00000
    mov eax,4*1024*16
    ;add edi,eax
    imul eax,[rel looptimes]
    add edi,eax
    ;lea ebx,[rel line0]
    mov ebx,0x6000
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


    mov esi,0x1c00000
    mov edi,[0x3028]
    mov bl,[0x3019]
    shr bl,3
    movzx ebx,bl
    mov ecx,1024*768
.continuescreen:
    lodsd
    mov [edi],eax
    add edi,ebx
    loop .continuescreen

jmp forever
;________________________________________
looptimes:dd 0








linex128:dq 0
length:dq 8
