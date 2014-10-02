;1.initialization
%include "init/acpi.s"
%include "init/pciold.s"
%include "init/isr.s"
%include "init/calldriver.s"
%include "init/variety.s"




;2.screen and memory manager
%include "dowhat/forever.s"	;有事找函数处理，无事睡觉
%include "dowhat/f1.s"		;内存每字节，一个个anscii或者hex显示到屏幕上
%include "dowhat/f2.s"		;内存每4B一个rgbw点,共1024*768*4=3MB图片
%include "dowhat/f3.s"		;内存每4B一个点,共1024*1024*1024*4=4G,3维图像
%include "dowhat/f4.s"		;喜闻乐见大终端
%include "dowhat/f5.s"		;屏幕设置失败，留在0xb8000文字模式，打印内存




;3.library
%include "library/power.s"	;poweroff,reboot
%include "library/work.s"	;f1~f5之间的切换
%include "library/char.s"	;点阵字符




;4.blank
times 0x2000-($-$$) db 0
endofjarvis:
