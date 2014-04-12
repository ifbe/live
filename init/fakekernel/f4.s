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
shl eax,7
add ebx,eax
add ebx,[rel length]
mov byte [ebx],' '

jmp console
;_________________________________________


;_____________________________________
other4:

cmp byte [rel length],128
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
shl edx,7
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
shl esi,7
lea edi,[rel line0]
add esi,edi
mov [rel backup],esi		;esi=current argument

mov esi,[rel backup]
cmp dword [esi],"powe"		;if [esi]=="powe"
jne skipturnoff
cmp dword [esi+4],"roff"	;if [esi+4]=="roff"
jne skipturnoff
cmp byte [esi+8],' '		;if [esi+8]=0x20
je turnoff
skipturnoff:

mov esi,[rel backup]
cmp dword [esi],"addr"		;if [esi]="addr"
jne skipaddr
cmp byte [esi+4],'='		;if [esi+4]="="
je changeaddr
skipaddr:


mov esi,[rel backup]
cmp dword [esi],"call"
jne skipcast
cmp byte [esi+4],0x20
je cast
skipcast:

mov esi,[rel backup]
cmp dword [esi],"jump"
jne skipjump
cmp byte [esi+4],0x20
je jump
skipjump:

mov esi,[rel backup]
cmp word [esi],"ls"
jne skipls
cmp byte [esi+2],0x20
je ls
skipls:

mov esi,[rel backup]
cmp word [esi],"cd"
jne skipcd
cmp byte [esi+2],0x20
je cd
skipcd:

mov esi,[rel backup]
cmp dword [esi],"load"
jne skipload
cmp byte [esi+4],0x20
je load
skipload:

notfound:
mov edi,[rel backup]
add edi,128
mov dword [edi],"notf"
mov dword [edi+4],"ound"
inc byte [rel linenumber]

scroll:
cmp byte [rel linenumber],0x1f
jb .ok

    lea esi,[rel line1]		;32行，往上挪！
    lea edi,[rel line0]
    mov ecx,128*0x20
    rep movsb
    jmp console

    .ok:
    inc byte [rel linenumber]	;加一
    jmp console
;_________________________________
backup:dq 0




;_________________________
ls:

inc byte [rel linenumber]   ;加一
mov edi,[rel backup]
add edi,128
mov esi,0x80000

mov ecx,16
.many:
movsq
add esi,0x18
add edi,8
loop .many

add byte [rel linenumber],2
jmp scroll
;_________________________




;________________________
cd:
cmp word [0x8008],"cd"
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
call [0x8000]
jmp scroll
;______________________




;________________________
load:
cmp dword [0x8018],"load"
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
call [0x8010]
jmp scroll
;______________________




;______________________
changeaddr:

add esi,5
call fetchvalue
mov rax,[rel temp]
mov [rel addr],rax
jmp scroll
;_________________________


;_______________________________
cast:

add esi,5
call fetchvalue
call [rel temp]
jmp scroll
;_______________________________


;_______________________________
jump:

add esi,5
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
.lines:
    mov edi,[rel ediforeground]
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
cmp byte [rel looptimes],0x20
jb .lines
mov dword [rel backcolor],0

call address
call writescreen
jmp forever
;________________________________________
edibackground:dd 0
ediforeground:dd 0
looptimes:dd 0
