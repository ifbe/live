[bits 64]
startofasm:



;[0x4000,0x4fff]
%include "realmode/still16.s"
%include "realmode/1024x768.s"
;%include "realmode/320x200.s"
;%include "realmode/80x25.s"
%include "realmode/enter64.s"

;[0x5000,0x5fff]
%include "longmode/0.paging.s"
%include "longmode/1.gdt.s"
%include "longmode/2.idt.s"
%include "longmode/3.lastword.s"
%include "longmode/4.library.s"




padding:
times 0x4000-(padding-startofasm) db 0




endofasm:
