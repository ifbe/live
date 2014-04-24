%include "jarvis/init.s"
%include "jarvis/forever.s"
%include "jarvis/esc.s"
%include "jarvis/f1.s"
%include "jarvis/f2.s"
%include "jarvis/f3.s"
%include "jarvis/f4.s"
%include "jarvis/global.s"
times 0x4000-($-$$) db 0


binaryahci:
incbin "ahci/temp"
