incbin "load/load"		;[0x0,0xffff]=[0,127]
times 0x10000-($-$$)db 0

incbin "init/init"		;[0x10000,0x1ffff]=[128,255]
times 0x100000-($-$$)db 0

incbin "demo/demo"		;[0x10000,0x1ffff]=[128,255]
times 0x200000-($-$$)db 0
