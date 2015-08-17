%include "chip/apic.s"

%include "exception/exception.s"
%include "exception/lastword.s"

%include "data/data.s"




times 0x2000-($-$$) db 0