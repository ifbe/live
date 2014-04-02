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
