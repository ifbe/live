%include "[8000,83ff]1024x768.s"
%include "[8400,87ff]longmode.s"
%include "[8800,8bff]acpi.s"
%include "[8c00,8fff]iopci.s"

;___________[9000,9fff]_____________
[bits 64]
startoffakekernel:

mov rdx,0xc000
call rdx
mov rdx,0xd000
call rdx
mov rdx,0x10000
call rdx
sleep:hlt
jmp sleep

endoffakekernel:
times 0x1000-(endoffakekernel-startoffakekernel) db 0
;___________________________________

%include "temp.s"

%include "[b000,bfff]anscii.s"

%include "[c000,cfff]exception.s"

%include "[d000,dfff]apic.s"
