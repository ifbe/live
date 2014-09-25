;((((((((((((((((((     part1:mykernel     ))))))))))))))))))
;jarvis is memory manager

%include "init.s"	;望名生意
%include "forever.s"	;有事找函数处理，无事睡觉
%include "global.s"	;一些广泛用到的函数
%include "f1.s"		;内存每字节，一个个anscii或者hex显示到屏幕上
%include "f2.s"		;内存每4B一个rgbw点,共1024*768*4=3MB图片显示到屏幕
%include "f3.s"		;内存每4B一个点,共1024*1024*1024*4=4G,3维图像映射到屏幕
%include "f4.s"		;喜闻乐见大终端
%include "f5.s"		;屏幕设置失败，留在0xb8000文字模式，打印内存来debug

;((((((((((((((((((((    endof part2    ))))))))))))))))))))

times 0x2000-($-$$) db 0
endofjarvis:
