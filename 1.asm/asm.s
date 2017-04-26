[bits 64]
startofasm:



times 0x400 db 0
%include "4400/still16.s"
%include "4600/1024x768.s"
%include "4800/enter64.s"
%include "4800/prepare64.s"
%include "5000/1.gdt.s"
%include "5000/2.idt.s"
%include "5000/3.lastword.s"
%include "5000/4.library.s"




padding:
times 0x4000-(padding-startofasm) db 0




endofasm:
