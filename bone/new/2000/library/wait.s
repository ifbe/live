%define kbdhome 0x5000
%define bufsize 0x1000
[bits 64]




;_________________________________________________
forever:			;不知被谁叫醒，一个个问一遍

.keyboard:			;键盘，有事？
	mov rax,[kbdhome+bufsize-8]
	cmp [kbdhome+bufsize-0x18],rax
	jne hellokeyboard

.mouse:				;鼠标，有事？
				;传感器，有事？......

.sleep:			;什么事都没有，那就睡觉吧，等着被唤醒
	hlt		;todo:不要一碰就醒，戳的使劲一些才醒就好了
	jmp forever
;_________________________________________________




;___________________________________
hellokeyboard:
	mov rax,[kbdhome+bufsize-8]
	mov al,[rax]

;更新dequque pointer
	inc qword [kbdhome+bufsize-8]               ;p++
	cmp qword [kbdhome+bufsize-8],kbdhome+bufsize-0x40    ;p>=0xfc0?
	jb .nochange
	mov qword [kbdhome+bufsize-8],kbdhome
.nochange:
	ret
;___________________________________
