%include "1-1024x768.s"
%include "2-longmode.s"
%include "3-acpi.s"
%include "4-iopci.s"
%include "5-disk.s"
%include "6-usb.s"
%include "7-anscii.s"
%include "8-exception.s"
%include "9-pic.s"
mov rdx,0x20000
call rdx
