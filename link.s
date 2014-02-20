;[0x0,0x7fff]=[0,63]
incbin "load/load"
times 32768-($-$$)db 0

;[0x8000,0xffff]=[64,127]
incbin "init/init"
times 65536-($-$$)db 0

;[0x10000,0x1ffff]=[128,255]
incbin "kernel/kernel"
times 131072-($-$$)db 0

;[0x20000,0x2ffff]=[256,383]
incbin "kernel/temp"
times 196608-($-$$)db 0

;[0x30000,0x3ffff]=[384,511]
incbin "kernel/temp1"
times 262144-($-$$)db 0

;[0x40000,0x7ffff]=[512,1023]
incbin "else/jarvis.jpg"
;times 524288-($-$$)db 0
times 589824-($-$$)db 0
