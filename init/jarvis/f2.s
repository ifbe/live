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
mov rax,[rel offset]
mov bl,0x40
div bl
cmp ah,0
je .equal
dec qword [rel offset]
jmp picture
    .equal:
    cmp qword [rel addr],0x800
    jb picture
    sub qword [rel addr],0x800
    jmp picture

f2right:
mov rax,[rel offset]
mov bl,0x40
div bl
cmp ah,0x3f
je .equal
inc qword [rel offset]
jmp picture
    .equal:
    add qword [rel addr],0x800
    jmp picture

f2up:
cmp qword [rel offset],0x40
jb .below
sub qword [rel offset],0x40
jmp picture
    .below:
    cmp qword [rel addr],0x40
    jb picture
    sub qword [rel addr],0x40
    jmp picture

f2down:
cmp qword [rel offset],0xbbf
ja .above
add qword [rel offset],0x40
jmp picture
    .above:
    add qword [rel addr],0x40
    jmp picture

;________________________________________




;____________________________________________
picture:
mov rsi,[rel jpegbase]
mov edi,0x1000000
mov ecx,1024*768

cld
rep movsd

call writescreen
jmp forever
;_________________________________________
jpegbase:dq 0
