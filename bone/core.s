;addr=0,size=1000
;[0,fff]
incbin "cpu.bin"
times 0x1000-($-$$) db 0




;addr=1000,size=1000
;[1000,1fff]
%include "1000/data.s"
times 0x2000-($-$$) db 0




;addr=2000,size=2000
;[2000,3fff]
incbin "trunk.bin"
times 0x4000-($-$$) db 0




;addr=4000,size=4000
;[4000,7fff]
incbin "extremities.bin"
times 0x8000-($-$$) db 0




;addr=8000,size=8000
;[8000,ffff]
times 0x10000-($-$$) db 0