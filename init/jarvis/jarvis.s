;(((((((((((((((     part1:mykernel     (((((((((((((((

;[+4000,+7fff]:
%include "init.s"
%include "forever.s"
%include "f1.s"
%include "f2.s"
%include "f3.s"
%include "f4.s"
%include "global.s"

;))))))))))))))))))))    endof part2    )))))))))))))

times 0x4000-($-$$) db 0
endofjarvis:
