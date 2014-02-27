%include "apic.s"
[BITS 64]
;____________________________________
start:
mov esi,0x20000
mov edi,0x100000
mov ecx,0x2000
rep movsq

;________________________________________
jpegdecode:                   ;位置无关
    cli
    mov rdx,0x100000            ;eax 传参
    call rdx
    mov [jpegbase],rax        ;return value
mov [0xc000],rax
    sti

;______________________________________________
prepare:
    xor rax,rax
    mov [0xff0],rax         ;keyboard buffer end
    mov ah,8
    mov [0xff8],rax
    mov r14,rax             ;r14 memory pointer
    xor r15,r15             ;r15 offset pointer

    mov edi,0x800
    mov [0xff8],edi
    xor rax,rax
    mov ecx,0xfe
    rep stosq

    jmp screen
;_______________________________________




;_________________main____________________
forever:
hlt                     ;wait for keyboard interrupt
cmp byte [0xff0],0
je forever              ;its not keyboard interrupt
mov byte [0xff0],0      ;clear keyboard flag

mov eax,[0xff8]
cmp ax,0x800
je forever
dec eax
mov al,[eax]

switchkey:

;part1:arrowkey........
cmp al,0x4b
je left
cmp al,0x4d
je right
cmp al,0x48
je up
cmp al,0x50
je down

;part2:specialkey........
;cmp al,0x01
;je esc
cmp al,0x1d
je ctrl
cmp al,0x3b
je f1
cmp al,0x3c
je f2
cmp al,0x3d
je f3
cmp al,0x3e
je f4
cmp al,0x0f
je tab

;part3:content........
cmp al,0x1c
je enter
cmp al,0x0e
je backspace
jmp other

;part4:display........
screen:

background:
    call ramdump
foreground:
    call console
somethingelse:
    call mouse
writetoscreen:
    call composite
jmp forever
;_________________________________


;_____________________________
left:
mov rax,r15
mov bl,0x40
div bl
cmp ah,0
je .left
dec r15
jmp screen
    .left:
    sub r14,0x800
    jmp screen

right:
mov rax,r15
mov bl,0x40
div bl
cmp ah,0x3f
je .right
inc r15
jmp screen
    .right:
    add r14,0x800
    jmp screen

up:
cmp r15,48
jb .up
sub r15,0x40
jmp screen
    .up:
    sub r14,0x40
    jmp screen

down:
cmp r15,0xbbf
ja .down
add r15,0x40
jmp screen
    .down:
    add r14,0x40
    jmp screen
;___________________________________


;___________________________________
ctrl:
inc byte [ctrlkey]
test byte [ctrlkey],1
jnz .off
    .on:
    mov dword [foreground+1],console-(foreground+5)              ;selfmodify
    jmp screen
    .off:
    mov dword [foreground+1],donothing-(foreground+5)            ;selfmodify
    jmp screen

tab:
inc byte [tabkey]
test byte [tabkey],1
jnz .anscii
    .hex:
    mov dword [dumpwhat-4],dumphex-dumpwhat                      ;selfmodify
    jmp screen
    .anscii:
    mov dword [dumpwhat-4],dumpanscii-dumpwhat                   ;selfmodify
    jmp screen

f1:
    mov dword [background+1],ramdump-(background+5)              ;selfmodify
    jmp screen
f2:
    mov dword [background+1],picture-(background+5)              ;selfmodify
    jmp screen
f3:
    jmp screen
f4:
    jmp screen

esckey db 0
commandkey db 0
tabkey db 0
capslockkey db 0
shiftkey db 0
ctrlkey db 0
altkey db 0
superkey db 0
;_______________________________________


;_____________________________________
scroll:
cmp byte [linenumber],0x1f
jb .easy

    mov esi,line2
    mov edi,line1
    mov ecx,64*0x1f
    rep movsb
    mov byte [length],0
    ret

    .easy:
    inc byte [linenumber]
    mov eax,[linenumber]
    shl eax,6
    mov edi,message
    add edi,eax
    mov [changebackspace+1],edi
    mov [changeother+1],edi
    mov byte [length],0
    ret
;___________________________________


;___________________________________
understand:
mov esi,[linenumber]
shl esi,6
add esi,message
mov [command],esi

mov edi,poweroff
mov ecx,9
repe cmpsb
je turnoff

mov esi,[command]
cmp dword [esi],"mov "
je ismov


.notfound:
mov edi,[command]
mov dword [edi+32],"notf"
mov dword [edi+36],"ound"
ret
;_________________________________
command:dd 0
poweroff:db "poweroff "


;______________________
ismov:
cmp dword [esi+4],"r14,"    ;49be
jne .maybef
mov byte [.tobemoved+1],0xbe
jmp .changeaddress

.maybef:
cmp dword [esi+4],"r15,"    ;49bf
jne understand.notfound
mov byte [.tobemoved+1],0xbf

.changeaddress:
add esi,8
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


;____________________________________
enter:
call understand
call scroll
jmp screen

backspace:
cmp byte [length],0
jna screen

dec byte [length]
changebackspace:
mov eax,line1
add eax,[length]
mov byte [eax],' '
jmp screen

other:
cmp byte [length],64
jae screen

call translate
changeother:
mov ebx,line1
add ebx,[length]
mov [ebx],al
inc byte [length]
jmp screen
;___________________________________


;____________________________
turnoff:
mov dx,[0x4fc]
mov ax,[0x4fe]
or ax,0x2000
out dx,ax
;__________________________


;_____________________________________
mouse:
mov esi,0xb8b0                 ;kuang
mov edi,0x100000
;mov edi,[0x3028]
mov eax,r15d
mov bl,0x40
div bl

movzx ebx,ah           ;余数
imul ebx,32
shl ebx,1
add edi,ebx

movzx eax,al           ;商
imul eax,4*1024*16
add edi,eax
call utf8
ret
;____________________________________


;____________________________________________
picture:
mov rsi,[jpegbase]
mov edi,0x100000
mov ecx,1024*768

cld
rep movsd

ret

jpegbase:dq 0
;_________________________________________


;_______________________________
;in:r14=memorystart,edi=destination
;change:r14+64,rax,rdx,cl
dumpanscii:

mov ecx,0x40
.dump:
    mov dword [frontcolor],0xff00
    mov esi,0xb200
    call char

    mov al,[r14]
    cmp al,0x20
    jb .blank
    cmp al,0x7e
    jb .visiable

    .blank:
    mov esi,0xb200
    jmp .normal

    .visiable:
    call number
    .normal:
    call char
    inc r14

dec cl
jnz .dump
ret
;_______________________________


;_______________________________
;in:r14=memorystart,edi=destination
;change:r14+64,rax,rdx,cl
dumphex:

mov cl,0x10
.dump4:
mov eax,[r14]
mov [save32],eax
add r14,4

  mov ch,4
  .dump:
  mov al,[save32]
  shr al,4
  call number
  call char
  mov al,[save32]
  and al,0xf
  call number
  call char
  shr dword [save32],8
  dec ch
  cmp ch,0
  jne .dump

dec cl
jnz .dump4

ret

save32:dd 0
;_______________________________


;______________________________
ramdump:
push r14
mov dword [frontcolor],0xff00
mov dword [linecount],0

dumpline:
    mov edi,0x100000        ;locate destination
    mov eax,[linecount]
    imul eax,4*1024*16
    add edi,eax
    call dumphex
dumpwhat:

inc byte [linecount]
cmp byte [linecount],0x30
jb dumpline

pop r14
ret

linecount:dd 0
;___________________________________


;___________________________________
donothing:
ret
;_____________________________________


;______________________________________
composite:
    mov esi,0x100000
    mov edi,[0x3028]
    mov bl,[0x3019]
    shr bl,3
    movzx ebx,bl
    mov ecx,1024*768

.continue:
    lodsd
    mov [edi],eax
    add edi,ebx
    loop .continue

ret
;________________________________________


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
    shr byte [edi],1        ;blue
    add byte [edi],0x20
    shr byte [edi+1],1        ;green
;    add byte [edi+1],0x1
    shr byte [edi+2],1        ;red
;    add byte [edi+2],0x1
    add edi,4
loop .first
mov eax,1024*4
add [edibackground],eax
dec edx
jnz .second

lea rbx,[r14+r15]
mov edi,line0+12
mov cl,16
.getaddress:
rol rbx,4
mov al,bl
and al,0x0f
stosb
dec cl
jnz .getaddress

mov dword [frontcolor],0xff0000
mov dword [backcolor],0xffffffff
mov dword [looptimes],0
.tmd:
    mov edi,[ediforeground]
    mov eax,4*1024*16
    ;add edi,eax
    imul eax,[looptimes]
    add edi,eax
    mov ebx,message
    mov eax,[looptimes]
    shl eax,6
    add ebx,eax
    call talk
inc byte [looptimes]
cmp byte [looptimes],0x20
jb .tmd
mov dword [backcolor],0

ret
edibackground:dd 0
ediforeground:dd 0
looptimes:dd 0
;________________________________________


;_________________________________
talk:
mov cl,64
.continue:
mov al,[ebx]
call number
call char
inc ebx
dec cl
jnz .continue
ret

linenumber:dd 0x01
length:dd 0
message:
line0:db "location=>                      time    =>     unknown          "
;line1:db "              at your service          (.o0.0o.)                "
line1:times 64 db ' '
line2:times 64 db ' '
line3:times 64 db ' '
line4:times 64 db ' '
line5:times 64 db ' '
line6:times 64 db ' '
line7:times 64 db ' '
line8:times 64 db ' '
line9:times 64 db ' '
linea:times 64 db ' '
lineb:times 64 db ' '
linec:times 64 db ' '
lined:times 64 db ' '
linee:times 64 db ' '
linef:times 64 db ' '
line10:times 64 db ' '
line11:times 64 db ' '
line12:times 64 db ' '
line13:times 64 db ' '
line14:times 64 db ' '
line15:times 64 db ' '
line16:times 64 db ' '
line17:times 64 db ' '
line18:times 64 db ' '
line19:times 64 db ' '
line1a:times 64 db ' '
line1b:times 64 db ' '
line1c:times 64 db ' '
line1d:times 64 db ' '
line1e:times 64 db ' '
line1f:times 64 db ' '
linenull:times 64 db ' '
;__________________________________


;_______________________________
;changed:ax,esi
number:
    xor ah,ah
    shl ax,4
    mov esi,0xb000                     ;zero
    add si,ax
    ret
;______________________________


;____________________________________________
;in:al
;change:esi,al
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
    ret
;__________________________________________


;进:esi,edi
;出:esi,edi
;用:
;变:
;______________________________
char:
    push rbx
    push rcx

    mov ecx,16           ;16行
.yihang:
    xor rax,rax
    lodsb
    push rcx
        mov cl,8
      .yidian:
        shl al,1
        jnc .bg
        mov edx,[frontcolor]
        mov [edi],edx
        jmp .pointdone
      .bg:
        mov edx,[backcolor]
        cmp edx,0xffffffff
        je .pointdone
        mov dword [edi],edx
      .pointdone:
        add edi,4
        loop .yidian
    sub edi,32           ;每个字的行头
    add edi,1024*4            ;下1行
    pop rcx
    loop .yihang

    add edi,32            ;下个字的行头
    sub edi,4*1024*16            ;上16行;现在edi=下个字开头

    mov eax,edi
    mov ebx,0x100000        ;ebx=vesabase
    ;mov ebx,[0x3028]        ;ebx=vesabase
    sub eax,ebx             ;eax=相对距离

.modfour:
    and eax,0x00000fff           ;mod4096
    cmp eax,0
    jne .nochange

.change:
    add edi,4*1024*16
    sub edi,1024*4

.nochange:
    pop rcx
    pop rbx
    ret              ;edi指向下一个字

backcolor:dd 0
frontcolor:dd 0xffffffff
;______________________________________


;进:esi,edi
;出:edi
;用:
;变:
utf8:
;_______________________________
    push rcx
    xor rcx,rcx
    mov cl,16
.yihang:
    xor rax,rax
    lodsw
    push rcx
        mov cl,16
      .yidian:
        shr ax,1
        jnc .transparent
        mov dword [edi],0xffffffff
      .transparent:
        add edi,4
        loop .yidian
    sub edi,64           ;每个字的行头
    add edi,1024*4            ;下1行
    pop rcx
    loop .yihang

    add edi,64            ;下个字的行头
    sub edi,4*1024*16            ;上16行;现在edi=下个字开头

    mov eax,edi
    mov ebx,0x100000        ;ebx=vesabase
    ;mov ebx,[0x3028]        ;ebx=vesabase
    sub eax,ebx             ;eax=相对距离

.modfour:
    and eax,0x00000fff           ;mod4096
    cmp eax,0
.change:
    add edi,4*1024*16
    sub edi,1024*4
.nochange:
    pop rcx
    ret              ;edi指向下一个字
;__________________________________
