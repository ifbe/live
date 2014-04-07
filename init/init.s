;our bootloader load initer to [where,where+ffff]
;memory layout of initer is as below
;you can choose oner or use yours (but be careful of length)
;my code is position independent

;[where,where+0x3ff]:			;below explain what the file do
%include "00/1024x768.s"		;put memory info in [2000,2fff]
					;put vesa info in [3000,3fff]
					;set screen mode with bios

;[where+400,where+7ff]:
%include "01/longmode.s"		;switch from 16bit to 64bit
					;enable float point
					;virtual mem=physical mem

;[where+800,where+bff]:
%include "02/old.s"	;or new		;put acpi info in [4000,4fff]

;[where+c00,where+fff]:
%include "03/apic.s"			;init localapic to all off
					;init ioapic to all off

;[where+1000,where+1fff]:
%include "04/exception.s"		;set 32 default exception handler
					;call read disk

;[where+2000,where+2fff]:
%include "1/help.s"			;E=mc^2......

;[where+3000,where+3fff]:
%include "2/anscii.s"			;anscii pixel table

;[where+4000,where+7fff]:
incbin "ahci/temp"			;read file and throw in memory
times 0x8000-($-$$) db 0

;[where+8000,where+ffff]:
incbin "fakekernel/temp"		;fake kernel,dump ram,console.(debug)
times 0x10000-($-$$) db 0
