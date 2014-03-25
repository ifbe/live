;our bootloader load initer to [8000,ffff]
;memory layout of initer is as below
;you can choose one
;or use yours (but be careful of length and deal with call.s)

;org 0x8000  ;position independent	;below explain what it do
;[0,0x3ff]:
%include "0/1024x768.s"			;put memory info in [2000,2fff]
					;put vesa info in [3000,3fff]
					;set screen mode with bios

;[400,7ff]:
%include "1/longmode.s"			;switch from 16bit to 64bit
					;enable float point
					;virtual mem=physical mem

;[800,9ff]:
%include "2/acpi.s"			;put acpi info in [4000,4fff]

;[a00,bff]:
%include "3/iopci.s"	;or mmpci	;put pci info in [5000,5fff]

;[c00,dff]:
%include "4/localapic.s"		;init localapic to all off

;[e00,fff]:
%include "5/ioapic.s"	;or pic		;init ioapic to all off

;[1000,1fff]:
%include "6/exception.s"		;set 32 default exception handler
					;call read disk

;[2000,2fff]:
%include "7/temp.s"			;E=mc^2......

;[3000,3fff]:
%include "8/anscii.s"			;anscii pixel table

;[4000,7fff]:
incbin "9/temp"				;read disk and ......
