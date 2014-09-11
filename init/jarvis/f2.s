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
cmp al,0x3f
je f5
cmp al,0x48
je f2up
cmp al,0x50
je f2down
jmp picture
;________________________________________




;________________________________________
f2up:
sub qword [rel jpegbase],0x10000
jmp picture
f2down:
add qword [rel jpegbase],0x10000
jmp picture
;_______________________________________




;________________________________________
picture:
mov rsi,[rel jpegbase]
mov edi,0x1000000
mov ecx,1024*768
cld
rep movsd

mov edi,0x1000000
mov rbx,[rel jpegbase]
call dumprbx
call writescreen
jmp forever
;_________________________________________
jpegbase:dq 0x1400000
