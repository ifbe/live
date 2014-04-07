bits 64
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-----F3----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
function3:
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

cyberspace:
call address
call writescreen
jmp forever
