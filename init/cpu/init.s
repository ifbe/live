;(((((((((((((((((    part0:init cpu    (((((((((((((((((

;our bootloader load initer to [anywhere,anywhere+ffff](anywhere<0x40000)
;you'd better not modify part1......except you know what you are doing......


;[+0,+ff]:
%include "0th/stillin16.s"	;put memory info in [2000,2fff]
				;put vesa info in [3000,3fff]

;[+100,+1ff]
;%include "1th/text.s"	;set screen mode with bios(real mode)
%include "1th/1024x768.s"	;set screen mode with bios(real mode)

;[+200,+2ff]:
%include "2th/enter64.s"	;switch from 16bit to 64bit
				;enable float point

;[+300,+3ff]:
%include "3th/prepare64.s"	;reload cr3,access to 16GB ram garanteed

;[+400,+7ff]:
%include "4th/apic.s"		;init apic to well known state

;[+800,+fff]:
%include "5th/exception.s"	;set 32 default exception handler

;[+1000,+1fff]:
%include "anscii.s"		;anscii pixel table

;)))))))))))))))))))) endof part1(16KB) ))))))))))))))))))))

times 0x2000-($-$$) db 0

endofcpu:
