;1.initialization
%include "init1/input.s"	;prepare interrupt
%include "init2/acpi.s"		;prepare 0x20000
%include "init3/anscii.s"	;prepare 0x30000
%include "init4/pciold.s"	;prepare 0x40000
%include "init5/usb.s"		;prepare 0x50000
%include "init6/bin.s"		;prepare 0x60000
%include "init7/console.s"	;prepare 0x70000
%include "init7/journal.s"	;prepare 0x70000
%include "initelse/else.s"	;call c program and enter final loop




;2.library
%include "lib/power.s"	;poweroff,reboot
%include "lib/work.s"	;f1~f5之间的切换
%include "lib/char.s"	;点阵字符
%include "lib/wait.s"	;有事找函数处理，无事睡觉




;3.screen and memory manager
%include "main/f1.s"		;内存每字节，一个个anscii或者hex显示到屏幕上
%include "main/f2.s"		;内存每4B一个rgbw点,共1024*768*4=3MB图片
%include "main/f3.s"		;内存每4B一个点,共1024*1024*1024*4=4G,3维图像
%include "main/f4.s"		;喜闻乐见大终端
%include "main/f5.s"		;屏幕设置失败，留在0xb8000文字模式，打印内存




;4.blank
times 0x2000-($-$$) db 0
endofjarvis:
