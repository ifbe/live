bits 64
;>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>-----F5----<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
function5:
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

cmp al,0x4b
je f5left
cmp al,0x4d
je f5right
cmp al,0x48
je f5up
cmp al,0x50
je f5down
jmp fallback

f5left:
mov rax,[rel offset]
mov bl,0x40
div bl
cmp ah,0
je .equal
dec qword [rel offset]
jmp fallback
    .equal:
    cmp qword [rel addr],0x800
    jb fallback
    sub qword [rel addr],0x800
    jmp fallback

f5right:
mov rax,[rel offset]
mov bl,0x40
div bl
cmp ah,0x3f
je .equal
inc qword [rel offset]
jmp fallback
    .equal:
    add qword [rel addr],0x800
    jmp fallback

f5up:
cmp qword [rel offset],0x40
jb .below
sub qword [rel offset],0x40
jmp fallback
    .below:
    cmp qword [rel addr],0x40
    jb fallback
    sub qword [rel addr],0x40
    jmp fallback

f5down:
cmp qword [rel offset],0x5bf
ja .above
add qword [rel offset],0x40
jmp fallback
    .above:
    add qword [rel addr],0x40
    jmp fallback




fallback:

mov rsi,[rel addr]
mov rdi,0xb8000
mov ah,0x7		;color
.lines:
mov rcx,0x40
	.oneline:
	lodsb
	stosw
	loop .oneline
	mov rcx,0x10
	mov al,0x20
	rep stosw
cmp rdi,0xb8f00
jb .lines

mov rbx,[rel addr]
call putrbx
mov rbx,[rel offset]
call putrbx

mov rax,[rel offset]
mov bl,0x40
div bl			;商是al，余数是ah
mov bl,ah
movzx rbx,bl		;余数
movzx rax,al		;商
imul rax,0xa0
shl rbx,1
mov rdi,0xb8000
add rdi,rax
add rdi,rbx
mov byte [rdi+1],0xf0

jmp forever




putrbx:
mov rcx,16
mov ah,0xf
mov byte [rel .nonzero],0
.continue:
rol rbx,4
mov al,bl
and al,0xf
	cmp byte [rel .nonzero],1
	je .normal
	cmp al,0
	jne .notnormal
	mov al,0x20
	jmp .skip
	.notnormal:
	mov byte [rel .nonzero],1
.normal:
add al,0x30
cmp al,0x3a
jb .skip
add al,0x7
.skip:
stosw
loop .continue

.return:
ret

.nonzero:db 0
