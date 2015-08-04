;[0,fff]
incbin "cpu.bin"
times 0x1000-($-$$) db 0




;[1000,1fff]
%include "1000/data.s"
times 0x2000-($-$$) db 0




;[2000,3fff]
incbin "trunk.bin"
times 0x4000-($-$$) db 0




;[4000,7fff]
incbin "extremities.bin"
times 0x8000-($-$$) db 0




;[8000,ffff]
times 0x8000-($-$$) db 0
times 0x10000-($-$$) db 0