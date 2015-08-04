[bits 64]
startofpic:

;___________________8259_____________________
pic:
cli

icw1:
mov al,0x11
out 0x20,al
out 0xa0,al

icw2:
mov al,0x20
out 0x21,al
mov al,0x28
out 0xa1,al

icw3:
mov al,0x4
out 0x21,al
mov al,0x2
out 0xa1,al

icw4:
mov al,0x01                ;03=aeoi
out 0x21,al
out 0xa1,al

mov al,0xff
out 0x21,al
out 0xa1,al
;____________________________________

jmp endofpic

paddingofpic:
times 0x200-(paddingofpic-startofpic) db 0


endofpic:
