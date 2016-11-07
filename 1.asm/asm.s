[bits 64]
startofexception:



%include "1.gdt.s"
%include "2.idt.s"
%include "3.lastword.s"
%include "4.library.s"




padding:
times 0x1000-(padding-startofexception) db 0




endofexception:
