;____________________初始化_______________________
	call endofjarvis

	call f1init
	call f2init
	call f3init
	call f4init

	call processevent.showf4
;_______________________________________________








;_______________准备进入正式程序__________________
showsomething:
	call printworld

forever:
	call waitevent

dispatchevent:
	call processevent

againandagain:
	jmp showsomething
;_________________________________________________




;_________________________________________________
printworld:
	call [rel whosscreen]

	;do something(for example:start menu)
	;call printforeground
	;call showmouse

	ret
;___________________________________________________




;____________处理掉自己的事件,其他的分发给其他人做___________
processevent:
.f1:
	cmp al,0x3b
	jne .notf1
.showf1:
	lea rax,[rel f1show]
	mov [rel whosscreen],rax

	lea rax,[rel f1event]
	mov [rel whosevent],rax

	mov qword [rel f1changebg],0xffff			;默认全部刷上屏幕
	mov qword [rel f1changefg],0xffff			;默认全部刷上屏幕
	ret
.notf1:




.f2:
	cmp al,0x3c
	jne .notf2
.showf2:
	lea rax,[rel f2show]
	mov [rel whosscreen],rax

	lea rax,[rel f2event]
	mov [rel whosevent],rax

	ret
.notf2:




.f3:
	cmp al,0x3d
	jne .notf3
.showf3:
	lea rax,[rel f3show]
	mov [rel whosscreen],rax

	lea rax,[rel f3event]
	mov [rel whosevent],rax

	ret
.notf3:




.f4:
	cmp al,0x3e
	jne .notf4
.showf4:
	lea rax,[rel f4show]
	mov [rel whosscreen],rax

	lea rax,[rel f4event]
	mov [rel whosevent],rax

	mov qword [rel f4change],0xffff			;默认全部刷上屏幕
	ret
.notf4:




.f5:
	cmp al,0x3f
	jne .notf5
.showf5:
	lea rax,[rel f5show]
	mov [rel whosscreen],rax

	lea rax,[rel f5event]
	mov [rel whosevent],rax

	ret
.notf5:




.otherkey:
	jmp [rel whosevent]
;___________________________________________
whosscreen:dq 0
whosevent:dq 0
