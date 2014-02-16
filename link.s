;[0x0,0xffff]=[0,127]
incbin "load/load"
times 65536-($-$$)db 0

;[0x10000,0x1ffff]=[128,255]
incbin "init/init"
times 131072-($-$$)db 0

;[0x20000,0x2ffff]=[256,383]
incbin "kernel/kernel"
times 196608-($-$$)db 0

;[0x30000,0x37fff]=[384,447]
incbin "kernel/temp"
times 229376-($-$$)db 0

;[0x38000,0x3ffff]=[448,511]
incbin "kernel/temp1"
times 262144-($-$$)db 0

;[0x40000,0x7ffff]=[512,1023]
incbin "else/jarvis.jpg"
times 524288-($-$$)db 0
