;(((((((((((((((((    part0:init cpu    (((((((((((((((((

;our bootloader load initer to [anywhere,anywhere+ffff](anywhere<0x40000)
;you'd better not modify part1......except you know what you are doing......


;[+0,+0x3ff]:
%include "00/1024x768.s"		;put memory info in [2000,2fff]
					;put vesa info in [3000,3fff]
					;set screen mode with bios(real mode)

;[+400,+7ff]:
%include "01/longmode.s"		;switch from 16bit to 64bit
					;enable float point
					;virtual mem=physical mem

;[+800,+bff]:
%include "02/old.s"	;or new		;put acpi info in [4000,4fff]

;[+c00,+fff]:
%include "03/apic.s"			;init localapic to all off
					;init ioapic to all off

;[+1000,+1fff]:
%include "1/exception.s"		;set 32 default exception handler
					;call read disk

;[+2000,+2fff]:
%include "2/help.s"			;how to

;[+3000,+3fff]:
%include "3/anscii.s"			;anscii pixel table

;)))))))))))))))))))) endof part1(16KB) ))))))))))))))))))))

times 0x4000-($-$$) db 0
