;1.initialization
%include "init/init1/input.s"	;prepare interrupt
%include "init/init2/acpi.s"		;prepare 0x20000
%include "init/init3/anscii.s"	;prepare 0x30000
%include "init/init4/pciold.s"	;prepare 0x40000
%include "init/init5/usb.s"		;prepare 0x50000
%include "init/init6/bin.s"		;prepare 0x60000




;2.main
%include "main/main.s"		;main loop
%include "main/f1.s"		;一维
%include "main/f2.s"		;二维
%include "main/f3.s"		;三维
%include "main/f4.s"		;终端
%include "main/f5.s"		;要是屏幕设置失败，会留在0xb8000模式




;2.library
%include "library/power.s"	;poweroff,reboot
%include "library/char.s"	;点阵字符
%include "library/wait.s"	;有事找函数处理，无事睡觉





;4.blank
times 0x2000-($-$$) db 0
endofjarvis:
