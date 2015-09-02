;[+0,+1ff]:决定是直接往后跳还是自己读64k进内存
%include "0/mbr.s"
times 0x200-($-$$) db 0




;[+200,3ff]:仍在实模式，把内存排布和屏幕信息记录一下
%include "200/still16.s"        ;vesa info
times 0x400-($-$$) db 0




;[+400,+7ff]:最后一次调用bios中断来调屏幕，以后再也用不起来了
%include "400/1024x768.s"       ;set screen mode with bios
times 0x800-($-$$) db 0




;[+800,+fff]:进入长模式，准备长模式
%include "800/enter64.s"        ;switch 64bit,enable float point
%include "800/prepare64.s"      ;reload cr3,access to 16GB ram garanteed
times 0x1000-($-$$) db 0