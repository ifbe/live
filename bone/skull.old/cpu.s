;(((((((((((((((((    part0:init cpu    (((((((((((((((((

;our bootloader load initer to [anywhere,anywhere+ffff](anywhere<0x40000)
;you'd better not modify part1......except you know what you are doing......


;[+0,+1ff]:
%include "0th/mbr.s"		;bootmgr won't use here

;[+200,+3ff]:
%include "1th/still16.s"	;put memory info in [2000,2fff]
				;put vesa info in [3000,3fff]
%include "1th/1024x768.s"	;set screen mode with bios(real mode)

;[+400,+5ff]:
%include "2th/enter64.s"	;switch 64bit,enable float point
%include "2th/prepare64.s"	;reload cr3,access to 16GB ram garanteed

;[+600,+7ff]:
%include "3th/apic.s"		;init apic to well known state

;[+800,+fff]:
%include "4th/exception.s"	;set 32 default exception handler
%include "4th/lastword.s"	;black screen

;[+1000,+1fff]:
%include "5th/data.s"		;anscii pixel table
%include "5th/anscii.s"		;anscii pixel table

;)))))))))))))))))))) endof part1(16KB) ))))))))))))))))))))

times 0x2000-($-$$) db 0

endofcpu:
