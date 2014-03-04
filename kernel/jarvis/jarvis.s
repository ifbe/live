%include "apic.s"
[BITS 64]
;____________________________________
jpegprogram:
    mov esi,0x20000
    mov edi,0x100000
    mov ecx,0x2000
    rep movsq

jpegdecode:                   ;位置无关
    cli
    mov rdx,0x100000            ;eax 传参
    call rdx
    mov [jpegbase],rax        ;return value
    sti

prepare:
    mov r14,0x100000             ;r14 memory pointer
    xor r15,r15             ;r15 offset pointer

    mov rdi,0x800
    mov [0xff8],rdi

    xor rax,rax
    mov [0xff0],rax         ;keyboard buffer end

    mov ecx,0xfe
    rep stosq

    jmp screen
;_______________________________________




;_________________main.part0(who wake me up)____________________
forever:
hlt			;	sleep

cmp byte [0xff0],0	;	(keyboard interrupt)?
je forever		;	no{sleep again}

			;	yes{
mov byte [0xff0],0	;		clear signal
mov eax,[0xff8]		;		pointer=[0xff8]
cmp ax,0x800		;		(pointer=0x800(buffer head))?
je forever		;		yes{sleep again}
			;		no{
dec eax			;			pointer-1
mov al,[eax]		;			al=[pointer]


;_________________________________



;______________main.part2(what do you want)____________________
switchkey:

;specialkey........	;			(al)?
			;			{
cmp al,0x3b
je f1			;			0x3b:f1
cmp al,0x3c
je f2			;			0x3c:f2
cmp al,0x3d
je f3			;			0x3d:f3
cmp al,0x3e
je f4			;			0x3e:f4
cmp al,0x01
je esc			;			0x01:esc
cmp al,0x1d
je ctrl			;			0x1d:ctrl
cmp al,0x38
je alt			;			0x38:alt
;cmp al,0x3a
;je capslock		;			0x38:capslock
;cmp al,0x2a
;je shift		;			0x38:shift
;cmp al,0x0f
;je tab			;			0x0f:tab
;cmp al,0x5b
;je super		;			0x0f:super

nextswitch:
jmp consoleswitch	;			default:throw it to others
;jmp f1switch
			;			}
;____________________________________________________________




;_______________main.part3(print to screen)_______________________
screen:

background:
    call ramdump	;		bottom layer
foreground:
    call console	;		top layer

writetoscreen:
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

jmp forever
			;		}
			;	}
;________________________________________




;______________________________________________________________________
f1:
    mov dword [background+1],ramdump-(background+5)              ;selfmodify
    ;mov dword [nextswitch+1],f1switch-(nextswitch+5)              ;selfmodify
    jmp screen
f2:
    mov dword [background+1],picture-(background+5)              ;selfmodify
    ;mov dword [nextswitch+1],f2switch-(nextswitch+5)              ;selfmodify
    jmp screen
f3:
    jmp screen
f4:
    jmp screen


ctrl:
inc byte [ctrlkey]
test byte [ctrlkey],1
jnz .off
    .on:
    mov dword [nextswitch+1],consoleswitch-(nextswitch+5)        ;selfmodify
    mov dword [foreground+1],console-(foreground+5)              ;selfmodify
    jmp screen
    .off:
    mov dword [nextswitch+1],f1switch-(nextswitch+5)              ;selfmodify
    mov dword [foreground+1],donothing-(foreground+5)            ;selfmodify
    jmp screen


alt:
inc byte [altkey]
test byte [altkey],1
jnz .anscii
    .hex:
    mov dword [dumpwhat-4],dumphex-dumpwhat                      ;selfmodify
    jmp screen
    .anscii:
    mov dword [dumpwhat-4],dumpanscii-dumpwhat                   ;selfmodify
    jmp screen


esc:
turnoff:
mov dx,[0x4fc]
mov ax,[0x4fe]
or ax,0x2000
out dx,ax

;_______________________________________________________________________________
esckey db 0
commandkey db 0
tabkey db 0
capslockkey db 0
shiftkey db 0
ctrlkey db 0
altkey db 0
superkey db 0



;_______________________________________________________________________________
f1switch:
cmp al,0x4b
je f1left
cmp al,0x4d
je f1right
cmp al,0x48
je f1up
cmp al,0x50
je f1down
cmp al,0x39
je f1space

jmp screen


f1left:
mov rax,r15
mov bl,0x40
div bl
cmp ah,0
je .equal
dec r15
jmp screen
    .equal:
    cmp r14,0x800
    jb screen
    sub r14,0x800
    jmp screen

f1right:
mov rax,r15
mov bl,0x40
div bl
cmp ah,0x3f
je .equal
inc r15
jmp screen
    .equal:
    add r14,0x800
    jmp screen

f1up:
cmp r15,0x40
jb .below
sub r15,0x40
jmp screen
    .below:
    cmp r14,0x40
    jb screen
    sub r14,0x40
    jmp screen

f1down:
cmp r15,0xbbf
ja .above
add r15,0x40
jmp screen
    .above:
    add r14,0x40
    jmp screen

f1space:
    mov r14,0x800
    mov r15,0
    jmp screen
;___________________________________________________________________




;_________________________________________
f2switch:
;________________________________________




;____________________________________________
picture:
mov rsi,[jpegbase]
mov edi,0x100000
mov ecx,1024*768

cld
rep movsd

ret
;_________________________________________
jpegbase:dq 0




;___________________________________
donothing:
ret
;_____________________________________




;_______________________________________________
consoleswitch:

cmp al,0x1c
je consoleenter
cmp al,0x0e
je consolebackspace
jmp consoleother


consoleenter:
call understand
jmp screen

consolebackspace:
cmp byte [length],0
jna screen

dec byte [length]
delete1:
mov eax,line0
add eax,[length]
mov byte [eax],' '
jmp screen


consoleother:

cmp byte [length],64
jae screen

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
jmp screen
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

ret
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




displayaddress:
mov dword [frontcolor],0xffffffff

lea rbx,[r14]
mov edi,0x100000+4*1024*728+4*888
call dumprbx

lea rbx,[r14+r15]
mov edi,0x100000+4*1024*744+4*888
call dumprbx




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

mov ecx,16
.first:
xor edx,edx
	.second:
	not dword [edi+edx]
	add edx,4
	cmp edx,64
	jb .second
add edi,4*1024
loop .first

ret
;___________________________________
linecount:dd 0




;_______________________________
;in:r14=memorystart,edi=destination
;change:r14+64,rax,rdx,cl
dumpanscii:

mov ecx,0x40
.dump:
    mov dword [frontcolor],0xff00
    mov al,0x20
    call char

    mov al,[r14]
    cmp al,0x20
    jb .blank
    cmp al,0x7e
    jb .visiable

    .blank:
    mov al,0x20

    .visiable:
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
  call char
  mov al,[save32]
  and al,0xf
  call char
  shr dword [save32],8
  dec ch
  cmp ch,0
  jne .dump

dec cl
jnz .dump4

ret
;_______________________________
save32:dd 0




;________________________________
dumprbx:
mov ecx,16
.getaddress:
	rol rbx,4
	mov al,bl
	and al,0x0f

	push rbx
	push rcx
	call char
	pop rcx
	pop rbx


	dec cl
	jnz .getaddress
;____________________________________




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




;进:esi,edi
;出:esi,edi
;用:
;变:
;______________________________
char:
    push rbx
    push rcx

    movzx eax,al
    shl ax,4
    lea esi,[eax+0xb000]

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
;______________________________________
backcolor:dd 0
frontcolor:dd 0xffffffff
