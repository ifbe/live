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
mov rax,r15
mov bl,0x40
div bl
cmp ah,0
je .equal
dec r15
jmp ramdump
    .equal:
    cmp r14,0x800
    jb ramdump
    sub r14,0x800
    jmp ramdump

f1right:
mov rax,r15
mov bl,0x40
div bl
cmp ah,0x3f
je .equal
inc r15
jmp ramdump
    .equal:
    add r14,0x800
    jmp ramdump

f1up:
cmp r15,0x40
jb .below
sub r15,0x40
jmp ramdump
    .below:
    cmp r14,0x40
    jb ramdump
    sub r14,0x40
    jmp ramdump

f1down:
cmp r15,0xbbf
ja .above
add r15,0x40
jmp ramdump
    .above:
    add r14,0x40
    jmp ramdump

f1space:
    mov r14,0x6000
    mov r15,0
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
push r14
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

pop r14

mouse:
mov edi,0x1000000
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

call address
call writescreen
jmp forever
;___________________________________
linecount:dd 0




;_______________________________
;in:r14=memorystart,edi=destination
;change:r14+64,rax,rdx,cl
dumpanscii:

mov ecx,0x40
.dump:
    mov dword [rel frontcolor],0xff00
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
mov [rel save32],eax
add r14,4

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
