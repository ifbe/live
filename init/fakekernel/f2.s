bits 64
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-----F2----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;_________________________________________
function2:
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

cmp al,0x4b
je f2left
cmp al,0x4d
je f2right
cmp al,0x48
je f2up
cmp al,0x50
je f2down

f2left:
mov rax,r15
mov bl,0x40
div bl
cmp ah,0
je .equal
dec r15
jmp picture
    .equal:
    cmp r14,0x800
    jb picture
    sub r14,0x800
    jmp picture

f2right:
mov rax,r15
mov bl,0x40
div bl
cmp ah,0x3f
je .equal
inc r15
jmp picture
    .equal:
    add r14,0x800
    jmp picture

f2up:
cmp r15,0x40
jb .below
sub r15,0x40
jmp picture
    .below:
    cmp r14,0x40
    jb picture
    sub r14,0x40
    jmp picture

f2down:
cmp r15,0xbbf
ja .above
add r15,0x40
jmp picture
    .above:
    add r14,0x40
    jmp picture

;________________________________________




;____________________________________________
picture:
mov rsi,[rel jpegbase]
mov edi,0x1000000
mov ecx,1024*768

cld
rep movsd

call address
call writescreen
jmp forever
;_________________________________________
jpegbase:dq 0
