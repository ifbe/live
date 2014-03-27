;our bootloader load initer to [8000,ffff]
;memory layout of initer is as below
;you can choose one
;or use yours (but be careful of length and deal with call.s)

;org 0x8000  ;position independent	;below explain what it do
;[0,0x3ff]:
%include "00/1024x768.s"		;put memory info in [2000,2fff]
					;put vesa info in [3000,3fff]
					;set screen mode with bios

;[400,7ff]:
%include "01/longmode.s"		;switch from 16bit to 64bit
					;enable float point
					;virtual mem=physical mem

;[800,bff]:
%include "02/old.s"	;or new		;put acpi info in [4000,4fff]

;[c00,fff]:
%include "03/apic.s"			;init localapic to all off
					;init ioapic to all off

;[1000,1fff]:
%include "1/exception.s"		;set 32 default exception handler
					;call read disk

;[2000,2fff]:
%include "2/temp.s"			;E=mc^2......

;[3000,3fff]:
%include "3/anscii.s"			;anscii pixel table

;[4000,7fff]:
incbin "4/temp"				;read disk and ......
