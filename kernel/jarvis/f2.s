;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-----F2----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
;_________________________________________
function2:
;________________________________________




;____________________________________________
picture:
mov rsi,[jpegbase]
mov edi,0x100000
mov ecx,1024*768

cld
rep movsd

call writescreen

jmp forever

;_________________________________________
jpegbase:dq 0
