;our bootloader load initer to [8000,ffff]
;memory layout of initer is as below
;you can choose one
;or use yours (but be careful of length and deal with call.s)


					;hope below notes will help

%include "[8000,83ff]/1024x768.s"	;[2000,2fff]:memory info
					;[3000,3fff]:vesa info
					;set screen mode with bios

%include "[8400,87ff]/longmode.s"	;64bit mode,virtual mem=physical mem

%include "[8800,89ff]/acpi.s"		;[4000,4fff]:acpi table address info

%include "[8a00,8bff]/iopci.s"		;[5000,5fff]:pci address info
;%include "[8c00,8fff]/mmpci.s"

%include "[8c00,8dff]/localapic.s"
%include "[8e00,8fff]/ioapic.s"

%include "[9000,9fff]/exception.s"	;call 0xa000
					;call 0xb000
					;......








%include "[a000,afff]/temp.s"		;rubbish

%include "[b000,bfff]/anscii.s"		;anscii pixel table

incbin "[c000,ffff]/temp"
