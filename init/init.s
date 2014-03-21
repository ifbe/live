;our bootloader load initer to [8000,ffff]
;memory layout of initer is as below
;you can choose one
;or use yours (but be careful of length and deal with call.s)


					;hope below notes will help

%include "[8000,83ff]/1024x768.s"	;[2000,2fff]:memory info
					;[3000,3fff]:vesa info
					;set screen mode with bios

%include "[8400,87ff]/longmode.s"	;64bit mode,virtual mem=physical mem

%include "[8800,8bff]/acpi.s"		;[4000,4fff]:acpi table address info

%include "[8c00,8fff]/iopci.s"		;[5000,5fff]:pci address info
;%include "[8c00,8fff]/mmpci.s"

%include "[9000,9fff]/call.s"		;call 0xa000
					;call 0xb000
					;......








%include "[a000,afff]/temp.s"		;rubbish

%include "[b000,bfff]/anscii.s"		;anscii pixel table

%include "[c000,cfff]/exception.s"	;[1000,1fff]:idt
					;default exception service routines

%include "[d000,dfff]/apic.s"		;init localapic and ioapic
;%include "[d000,dfff]/pic.s"		;init 8259

incbin "[e000,efff]/temp"
times 0x7000-($-$$)db 0

incbin "[f000,ffff]/temp"
times 0x8000-($-$$)db 0
