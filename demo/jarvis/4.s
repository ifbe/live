;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>----console----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;_______________________________________________
function4:
cmp al,0x3b
je f1
cmp al,0x3c
je f2
cmp al,0x3d
je f3
cmp al,0x3e
je f4
cmp al,0x01
je esc

cmp al,0x1c
je consoleenter
cmp al,0x0e
je consolebackspace
jmp consoleother


consoleenter:
call understand
jmp console

consolebackspace:
cmp byte [length],0
jna console

dec byte [length]
delete1:
mov eax,line0
add eax,[length]
mov byte [eax],' '
jmp console


consoleother:

cmp byte [length],64
jae console

translate:
    mov esi,0xb800                 ;table
.search:
    cmp [esi],al            ;先把al里的扫描码转换成anscii给al
    je .convert
    add esi,2
    cmp esi,0xb880
    jb .search
    mov esi,0xb200           ;no,blank
    jmp .finish
.convert:
    inc esi
    lodsb
.finish:

record:
mov ebx,line0
add ebx,[length]
mov [ebx],al
inc byte [length]
jmp console
;_______________________________________________________________________________




;___________________________________________
console:
mov edi,0x100000

mov eax,1024*4
shl eax,6
add edi,eax

mov [edibackground],edi
mov [ediforeground],edi

mov edx,640
.second:
mov edi,[edibackground]
mov ecx,1024
.first:
    shr byte [edi],2        ;blue
    add byte [edi],0x20
    shr byte [edi+1],2        ;green
;    add byte [edi+1],0x1
    shr byte [edi+2],2        ;red
;    add byte [edi+2],0x1
    add edi,4
loop .first
mov eax,1024*4
add [edibackground],eax
dec edx
jnz .second

mov dword [frontcolor],0xff0000
mov dword [backcolor],0xffffffff
mov dword [looptimes],0
.tmd:
    mov edi,[ediforeground]
    mov eax,4*1024*16
    ;add edi,eax
    imul eax,[looptimes]
    add edi,eax
    mov ebx,line0
    mov eax,[looptimes]
    shl eax,6
    add ebx,eax
    call message
inc byte [looptimes]
cmp byte [looptimes],0x20
jb .tmd
mov dword [backcolor],0

call writescreen

jmp forever

;________________________________________
edibackground:dd 0
ediforeground:dd 0
looptimes:dd 0




;___________________________________
understand:
mov esi,[linenumber]
shl esi,6
add esi,line0
mov [saveesi],esi

mov edi,poweroff
mov ecx,9
repe cmpsb
je turnoff

mov esi,[saveesi]
cmp dword [esi],"r14="
je changer14


.notfound:
mov edi,[saveesi]
mov dword [edi+32],"notf"
mov dword [edi+36],"ound"


scroll:
cmp byte [linenumber],0x1f
jb .easy

    mov esi,line1
    mov edi,line0
    mov ecx,64*0x1f
    rep movsb
    mov byte [length],0
    ret

    .easy:
    inc byte [linenumber]
    mov eax,[linenumber]
    shl eax,6
    mov edi,line0
    add edi,eax
    mov [delete1+1],edi
    mov [record+1],edi
    mov byte [length],0

ret
;_________________________________
saveesi:dd 0
poweroff:db "poweroff "




;______________________
changer14:
mov byte [.tobemoved+1],0xbe

.changeaddress:
add esi,4
cmp byte [esi],0x20
je understand.notfound

xor rbx,rbx
mov ecx,16

.part:
	lodsb                ;get one char

	cmp al,0x30          ;<0x30,error
	jb understand.notfound
	cmp al,0x39          ;>0x39,maybe a~f
	ja .atof
	sub al,0x30          ;now its certainly 0~9
	jmp .generate

	.atof:
	cmp al,0x61          ;[0x40,0x60],error
	jb understand.notfound
	cmp al,0x66          ;>0x66,error
	ja understand.notfound
	sub al,0x57          ;now its certainly a~f

	.generate:
	add bl,al
	cmp byte [esi],0x20
	je .finish
	shl rbx,4

	loop .part

.finish:
mov [.tobemoved+2],rbx
.tobemoved:
mov r14,0x0123456789abcdef

.return:
ret
;_________________________




;_________________________________
message:
mov cl,64
.continue:
mov al,[ebx]
call char
inc ebx
dec cl
jnz .continue

ret
;__________________________________
linenumber:dd 0x00
length:dd 0
line0:times 64 db ' '
line1:times 64 db ' '
lines:times 64*30 db ' '
linenull:times 64 db ' '
