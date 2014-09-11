;(((((((((((((((((    part0:init cpu    (((((((((((((((((

;our bootloader load initer to [anywhere,anywhere+ffff](anywhere<0x40000)
;you'd better not modify part1......except you know what you are doing......


;[+0,+1ff]:
%include "0th/stillin16.s"	;put memory info in [2000,2fff]
				;put vesa info in [3000,3fff]

;[+200,3ff]
;%include "1th/text.s"	;set screen mode with bios(real mode)
%include "1th/1024x768.s"	;set screen mode with bios(real mode)

;[+400,+5ff]:
%include "2th/enter64.s"	;switch from 16bit to 64bit
				;enable float point

;[+600,+7ff]:
%include "3th/prepare64.s"	;reload cr3,access to 16GB ram garanteed

;[+800,+9ff]:
%include "4th/acpi.s"		;put acpi info in [4000,4fff]

;[+a00,+bff]:
%include "5th/pciold.s"		;put pci info in [5000,5fff]

;[+c00,+dff]:
%include "6th/localapic.s"	;init localapic to all off

;[+e00,+fff]:
%include "7th/ioapic.s"		;init ioapic to all off

;[+1000,+1fff]:
%include "8th/exception.s"	;set 32 default exception handler

;[+2000,+3fff]:
%include "9th/anscii.s"		;anscii pixel table

;)))))))))))))))))))) endof part1(16KB) ))))))))))))))))))))

times 0x4000-($-$$) db 0
