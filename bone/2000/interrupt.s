startofexception:




%include "chip/apic.s"

%include "exception/exception.s"
%include "exception/lastword.s"




times 0x2000-($-$$) db 0
endofexception: