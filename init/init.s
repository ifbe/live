;[8000,8fff]
%include "[8000,83ff]/1024x768.s"
%include "[8400,87ff]/longmode.s"
%include "[8800,8bff]/acpi.s"
%include "[8c00,8fff]/iopci.s"		;or mmpci.s

;[9000,9fff]
%include "[9000,9fff]/call.s"

;[a000,afff]
%include "[a000,afff]/temp.s"

;[b000,bfff]
%include "[b000,bfff]/anscii.s"

;[c000,cfff]
%include "[c000,cfff]/exception.s"

;[d000,dfff]
%include "[d000,dfff]/apic.s"		;or pic.s
