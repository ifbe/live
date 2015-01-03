;1.initialization
%include "wrap/bare/init/input.s"	;prepare 0x800
%include "wrap/bare/init/first.s"	;prepare 0x100000
%include "wrap/bare/init/anscii.s"	;prepare 0x110000
%include "wrap/bare/init/console.s"	;prepare 0x120000
%include "wrap/bare/init/journal.s"	;prepare 0x130000
%include "wrap/bare/init/pciold.s"	;prepare 0x140000
%include "wrap/bare/init/usb.s"		;prepare 0x150000
%include "wrap/bare/init/acpi.s"	;prepare 0x160000
%include "wrap/bare/init/bin.s"		;prepare 0x180000
%include "wrap/bare/init/else.s"	;call c program and enter final loop




;2.library
%include "wrap/bare/lib/power.s"	;poweroff,reboot
%include "wrap/bare/lib/work.s"	;f1~f5之间的切换
%include "wrap/bare/lib/char.s"	;点阵字符
%include "wrap/bare/lib/wait.s"	;有事找函数处理，无事睡觉




;3.screen and memory manager
%include "main/f1.s"		;内存每字节，一个个anscii或者hex显示到屏幕上
%include "main/f2.s"		;内存每4B一个rgbw点,共1024*768*4=3MB图片
%include "main/f3.s"		;内存每4B一个点,共1024*1024*1024*4=4G,3维图像
%include "main/f4.s"		;喜闻乐见大终端
%include "main/f5.s"		;屏幕设置失败，留在0xb8000文字模式，打印内存




;4.blank
times 0x2000-($-$$) db 0
endofjarvis:
