;____________________初始化_______________________
	call endofjarvis

	call f1init
	call f4init

	call processevent.isf4
;_______________________________________________








;_______________准备进入正式程序__________________
showsomething:
	call printworld

waitevent:
	call forever

dispatch:
	call processevent

againandagain:
	jmp showsomething
;_________________________________________________




;_________________________________________________
printworld:
	call [rel screenwhat]
	;do something(for example:start menu)
	ret
;___________________________________________________
screenwhat:dq 0




;____________处理掉自己的事件,其他的分发给其他人做___________
processevent:
.f1:
	cmp al,0x3b
	jne .notf1
.isf1:
	lea rax,[rel f1show]
	mov [rel screenwhat],rax
	lea rax,[rel f1event]
	mov [rel whoevent],rax
	ret
.notf1:

.f2:
	cmp al,0x3c
	jne .notf2
.isf2:
	lea rax,[rel f2show]
	mov [rel screenwhat],rax
	lea rax,[rel f2event]
	mov [rel whoevent],rax
	ret
.notf2:

.f3:
	cmp al,0x3d
	jne .notf3
.isf3:
	lea rax,[rel f3show]
	mov [rel screenwhat],rax
	lea rax,[rel f3event]
	mov [rel whoevent],rax
	ret
.notf3:

.f4:
	cmp al,0x3e
	jne .notf4
.isf4:
	lea rax,[rel f4show]
	mov [rel screenwhat],rax
	lea rax,[rel f4event]
	mov [rel whoevent],rax
	ret
.notf4:

.f5:
	cmp al,0x3f
	jne .notf5
.isf5:
	lea rax,[rel f5show]
	mov [rel screenwhat],rax
	lea rax,[rel f5event]
	mov [rel whoevent],rax
	ret
.notf5:

.otherkey:
	jmp [rel whoevent]
;___________________________________________
whoevent:dq 0