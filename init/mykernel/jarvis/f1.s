bits 64
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-----F1-----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;___________________________
function1:
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
;cmp al,0x1d
;je ctrl
;cmp al,0x38
;je alt
;cmp al,0x3a
;je capslock
;cmp al,0x2a
;je shift
;cmp al,0x0f
;je tab
;cmp al,0x5b
;je super

cmp al,0x39
je f1space
cmp al,0x4b
je f1left
cmp al,0x4d
je f1right
cmp al,0x48
je f1up
cmp al,0x50
je f1down
cmp al,0x38
je f1alt

jmp ramdump


f1left:
mov rax,[rel offset]
mov bl,0x40
div bl
cmp ah,0
je .equal
dec qword [rel offset]
jmp ramdump
    .equal:
    cmp qword [rel addr],0x800
    jb ramdump
    sub qword [rel addr],0x800
    jmp ramdump

f1right:
mov rax,[rel offset]
mov bl,0x40
div bl
cmp ah,0x3f
je .equal
inc qword [rel offset]
jmp ramdump
    .equal:
    add qword [rel addr],0x800
    jmp ramdump

f1up:
cmp qword [rel offset],0x40
jb .below
sub qword [rel offset],0x40
jmp ramdump
    .below:
    cmp qword [rel addr],0x40
    jb ramdump
    sub qword [rel addr],0x40
    jmp ramdump

f1down:
cmp qword [rel offset],0xbbf
ja .above
add qword [rel offset],0x40
jmp ramdump
    .above:
    add qword [rel addr],0x40
    jmp ramdump

f1space:
    mov qword [rel addr],0x6000
    mov qword [rel offset],0
    jmp ramdump

f1alt:
inc byte [rel altkey]
test byte [rel altkey],1
jnz .anscii
    .hex:
    mov dword [rel dumpwhat-4],dumphex-dumpwhat                      ;selfmodify
    jmp ramdump
    .anscii:
    mov dword [rel dumpwhat-4],dumpanscii-dumpwhat                   ;selfmodify
    jmp ramdump

;_______________________________________
altkey db 1




;______________________________
ramdump:
push qword [rel addr]
mov dword [rel frontcolor],0xff00
mov dword [rel linecount],0

dumpline:
    mov edi,0x1000000        ;locate destination
    mov eax,[rel linecount]
    imul eax,4*1024*16
    add edi,eax
    call dumpanscii
dumpwhat:

inc byte [rel linecount]
cmp byte [rel linecount],0x30
jb dumpline

pop qword [rel addr]

mouse:
mov edi,0x1000000
mov eax,[rel offset]
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

call address
call writescreen
jmp forever
;___________________________________
linecount:dd 0




;_______________________________
dumpanscii:

mov ecx,0x40
.dump:
    mov dword [rel frontcolor],0xff00
    mov al,0x20
    call char

    mov rax,[rel addr]
    mov al,[rax]
    cmp al,0x20
    jb .blank
    cmp al,0x7e
    jb .visiable

    .blank:
    mov al,0x20

    .visiable:
    call char
    inc qword [rel addr]

dec cl
jnz .dump
ret
;_______________________________




;_______________________________
dumphex:

mov cl,0x10
.dump4:
mov rax,[rel addr]
mov eax,[rax]
mov [rel save32],eax
add qword [rel addr],4

  mov ch,4
  .dump:
  mov al,[rel save32]
  shr al,4
  call char
  mov al,[rel save32]
  and al,0xf
  call char
  shr dword [rel save32],8
  dec ch
  cmp ch,0
  jne .dump

dec cl
jnz .dump4

ret
;_______________________________
save32:dd 0
