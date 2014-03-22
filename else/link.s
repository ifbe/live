incbin "load/load"		;[0x0,0x7fff]=[0,63]
times 0x8000-($-$$)db 0

incbin "init/init"		;[0x8000,0xffff]=[64,127]
times 0x10000-($-$$)db 0

incbin "demo/demo"		;[0x10000,0x3ffff]=[128,511]
times 0x40000-($-$$)db 0

incbin "else/jarvis.jpg"	;[0x40000,0x7ffff]=[512,1023]
;times 524288-($-$$)db 0
times 0x90000-($-$$)db 0
