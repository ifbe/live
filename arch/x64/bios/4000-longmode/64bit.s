startof64:
%include "0.paging.s"
%include "1.gdt.s"
%include "2.idt.s"
%include "3.lastword.s"
%include "4.library.s"




padding:
times 0x4000-(padding-startof64) db 0
endof64:
