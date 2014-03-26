incbin "load/load"		;[0x0,0xffff]=[0,127]
times 0x10000-($-$$)db 0

incbin "init/init"		;[0x10000,0x1ffff]=[128,255]
times 0x20000-($-$$)db 0

incbin "demo/demo"		;[0x10000,0x3ffff]=[255,511]
times 0x40000-($-$$)db 0

incbin "else/jarvis.jpg"	;[0x40000,0x7ffff]=[512,1023]
;times 524288-($-$$)db 0
times 0x90000-($-$$)db 0
