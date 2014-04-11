bits 64
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
je enter4
cmp al,0x0e
je backspace4

jmp other4
;_______________________________________________________________________________


;_______________________________
backspace4:

cmp byte [rel length],0
jna console

dec byte [rel length]
lea ebx,[rel line0]
mov eax,[rel linenumber]
shl eax,6
add ebx,eax
add ebx,[rel length]
mov byte [ebx],' '

jmp console
;_________________________________________


;_____________________________________
other4:

cmp byte [rel length],64
jae console

translate:
    mov esi,0x7800                 ;table
.search:
    cmp [esi],al            ;先把al里的扫描码转换成anscii给al
    je .convert
    add esi,2
    cmp esi,0x7880
    jb .search
    mov esi,0x7200           ;no,blank
    jmp .finish
.convert:
    inc esi
    lodsb
.finish:

record:
lea ebx,[rel line0]
mov edx,[rel linenumber]
shl edx,6
add ebx,edx
add ebx,[rel length]
mov [ebx],al
inc byte [rel length]
jmp console
;___________________________________


;___________________________________
enter4:

mov byte [rel length],0
mov esi,[rel linenumber]
shl esi,6
lea edi,[rel line0]
add esi,edi
mov [rel backup],esi		;esi=current argument

mov esi,[rel backup]
lea edi,[rel poweroff]
mov ecx,9
repe cmpsb
je turnoff			;its poweroff

mov esi,[rel backup]
cmp dword [esi],"addr"
je changeaddr

mov esi,[rel backup]
cmp dword [esi],"call"
je cast

mov esi,[rel backup]
cmp dword [esi],"jump"
je jump

mov esi,[rel backup]
cmp word [esi],"ls"
je ls

mov esi,[rel backup]
cmp word [esi],"cd"
je cd

mov esi,[rel backup]
cmp dword [esi],"load"
je load

notfound:
mov edi,[rel backup]
mov dword [edi+32],"notf"
mov dword [edi+36],"ound"
;____________________________________


;________________________________
scroll:
cmp byte [rel linenumber],0x1f
jb .ok

    lea esi,[rel line1]		;32行，往上挪！
    lea edi,[rel line0]
    mov ecx,64*0x20
    rep movsb
    jmp console

    .ok:
    inc byte [rel linenumber]	;加一
    jmp console
;_________________________________
backup:dq 0
poweroff:db "poweroff "




;_________________________
ls:

add esi,2
cmp byte [esi],' '
jne notfound

cmp byte [rel linenumber],0x1a
jb .skip
mov byte [rel linenumber],0   ;加一
.skip:
mov edi,[rel linenumber]
inc edi
shl edi,6
lea esi,[rel line0]
add edi,esi
mov esi,0x80000

mov ecx,16
.many:
movsq
add esi,0x18
add edi,8
loop .many

add byte [rel linenumber],4
jmp scroll
;_________________________




;________________________
cd:
jmp scroll
;______________________




;________________________
load:
jmp scroll
;______________________




;______________________
changeaddr:

add esi,4
cmp byte [esi],'='
jne notfound

inc esi
call fetchvalue
mov rax,[rel temp]
mov [rel addr],rax
jmp scroll
;_________________________


;_______________________________
cast:

add esi,4
cmp byte [esi],0x20
jne notfound

inc esi
call fetchvalue
call [rel temp]
jmp scroll
;_______________________________


;_______________________________
jump:

add esi,4
cmp byte [esi],0x20
jne notfound

inc esi
call fetchvalue
jmp [rel temp]
;_______________________________


;________________________________
fetchvalue:
mov qword [rel temp],0
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
add byte [rel temp],al
cmp byte [esi],0x20
je .finish
shl qword [rel temp],4

loop .part

.finish:
ret
;_______________________________-


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


;___________________________________________
console:
mov edi,0x1000000
mov eax,1024*4
shl eax,6
add edi,eax

mov [rel edibackground],edi
mov [rel ediforeground],edi

mov eax,0x1234
mov ecx,1024*640
rep stosd

mov dword [rel frontcolor],0xff0000
mov dword [rel backcolor],0xffffffff
mov dword [rel looptimes],0
.tmd:
    mov edi,[rel ediforeground]
    mov eax,4*1024*16
    ;add edi,eax
    imul eax,[rel looptimes]
    add edi,eax
    lea ebx,[rel line0]
    mov eax,[rel looptimes]
    shl eax,6
    add ebx,eax
    call message
inc byte [rel looptimes]
cmp byte [rel looptimes],0x20
jb .tmd
mov dword [rel backcolor],0

call address
call writescreen
jmp forever
;________________________________________
edibackground:dd 0
ediforeground:dd 0
looptimes:dd 0
