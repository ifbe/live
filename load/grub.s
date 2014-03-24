[bits 16]
mov ax,0xb800
mov es,ax
mov di,0

push cs
pop bx
mov ds,bx
mov si,0x8000
call printbx

mov al,':'
mov ah,0x7
stosw

call next	;push ip
next:pop bx
call printbx

xor ax,ax
mov es,ax
mov di,0x8000
mov cx,0x8000
rep movsb

jmp 0:0x8000




printbx:
mov al,bh
shr al,4
call printhex
mov al,bh
and al,0x0f
call printhex
mov al,bl
shr al,4
call printhex
mov al,bl
and al,0x0f
call printhex
ret

printhex:
add al,0x30
cmp al,0x3a
jb .return
add al,7
.return:
stosb
mov al,0xf0
stosb
ret

times 510-($-$$) db 0
dw 0xAA55
