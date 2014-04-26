;our bootloader load initer to [anywhere,anywhere+ffff](anywhere<0x40000)
;you'd better not modify part1......except you know what you are doing......


;(((((((((((((((((    part1:init cpu    (((((((((((((((((

;[+0,+0x3ff]:				;below explain what the file do
%include "00/1024x768.s"		;put memory info in [2000,2fff]
					;put vesa info in [3000,3fff]
					;set screen mode with bios

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















;you can delete all about part2 if you dont like my messy code
;and use your whatever 64bit code like this (remember,never ret):
;                             forever: hlt
;                             jmp forever


;(((((((((((((((    part2:filesystem    (((((((((((((((

;[+4000,+ffff]:
incbin "mykernel/temp"		;fake kernel,dump ram,console.(debug)
times 0x10000-($-$$) db 0

;))))))))))))))))))))    endof part2    )))))))))))))
