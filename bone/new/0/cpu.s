%include "0/mbr.s"
times 0x200-($-$$) db 0




;[+200,+3ff]:some thing must be done in realmode
%include "200/still16.s"        ;vesa info
%include "200/1024x768.s"       ;set screen mode with bios
;%include "200/test.s"  or 320*240.s or ......
times 0x400-($-$$) db 0




;[+400,+7ff]:longmode
%include "400/enter64.s"        ;switch 64bit,enable float point
%include "400/prepare64.s"      ;reload cr3,access to 16GB ram garanteed
%include "400/apic.s"           ;init apic to well known state
times 0x800-($-$$) db 0




;[+800,+fff]:blue screen for debug
%include "800/exception.s"      ;set 32 default exception handler
%include "800/lastword.s"       ;black screen
times 0x1000-($-$$) db 0
