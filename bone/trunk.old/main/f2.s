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

	mov edi,0x1400000

	mov rbx,[0x4fd8]
	cmp rbx,768
	jb .yok
	mov rbx,760
	.yok:
	shl rbx,2+10		;每行4096B=0x1000=2^12
	add rdi,rbx

	mov rax,[0x4fc8]		;鼠标
	cmp rax,1024
	jb .xok
	mov rax,1020
	.xok:
	shl rax,2
	add rdi,rax

	mov rcx,16
	.continue:
	mov rax,0x123456789abcdef
	mov [rdi],rax
	mov [rdi+8],rax
	mov [rdi+16],rax
	mov [rdi+24],rax
	mov [rdi+32],rax
	mov [rdi+40],rax
	mov [rdi+48],rax
	mov [rdi+56],rax
	add rdi,4096
	loop .continue


	mov esi,[rel jpegbase]
	mov edi,[0x3028]
	mov bl,[0x3019]
	shr bl,3
	movzx ebx,bl
	mov ecx,1024*768
.continuescreen:
	lodsd
	mov [edi],eax
	add edi,ebx
	loop .continuescreen

jmp forever
;_________________________________________
jpegbase:dq 0x1400000
