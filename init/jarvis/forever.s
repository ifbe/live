bits 64
;_________________________________________________
forever:			;不知被谁叫醒，一个个问一遍

.keyboard:			;键盘，有事？
mov rax,[0xff8]
cmp [0xfe8],rax
jne hellokeyboard

.mouse:				;鼠标，有事？
				;传感器，有事？......

.sleep:				;什么事都没有，那就睡觉吧，等着被唤醒
hlt				;todo:不要一碰就醒，戳的使劲一些才醒就好了
jmp forever
;_________________________________________________




;___________________________________
hellokeyboard:
;下一个要处理的数据
mov rax,[0xff8]
mov al,[rax]

;更新dequque pointer
inc qword [0xff8]               ;p++
cmp qword [0xff8],0xfc0         ;p>=0xfc0?
jb .nochange
mov qword [0xff8],0x800
.nochange:
;___________________________________


decide:
jmp [rel screenwhat]
screenwhat:dq 0
