;____________________初始化_______________________
	call f1init
	call f4init
	call endofjarvis                ;ahci@0x4000

	lea rax,[rel function4]
	mov [rel screenwhat],rax
	xor rax,rax
;_______________________________________________








;_______________准备进入正式程序__________________
showsomething:
	call [rel screenwhat]

waitevent:
	call forever

dispatch:
	call processevent

againandagain:
	jmp showsomething
;_________________________________________________








;____________处理掉自己的事件,其他的分发给其他人做___________
processevent:
.f1:
	cmp al,0x3b
	jne .notf1

        lea rax [rel ramdump]
	mov [rel screenwhat],rax
        lea rax,[rel function1]
        mov [rel whoevent],rax

	ret
.notf1:

.f2:
	cmp al,0x3c
	jne .notf2

        lea rax,[rel f2show]
	mov [rel screenwhat],rax
        lea rax,[rel f2event]
        mov [rel whoevent],rax

	ret
.notf2:

.f3:
	cmp al,0x3d
	jne .notf3

        lea rax,[rel f3show]
	mov [rel screenwhat],rax
        lea rax,[rel f3event]
        mov [rel whoevent],rax

	ret
.notf3:

.f4:
	cmp al,0x3e
	jne .notf4

	lea rax,[rel f4show]
	mov [rel screenwhat],rax
        lea rax,[rel f4event]
        mov [rel whoevent],rax

	ret
.notf4:

.f5:
	cmp al,0x3f
	jne .notf5

        lea rax,[rel fallback]
	mov [rel screenwhat],rax
        lea rax,[rel function5]
        mov [rel whoevent],rax

	ret
.notf1:

.otherkey:
	call [rel whoevent]
;___________________________________________








screenwhat:dq 0
whoevent:dq 0
