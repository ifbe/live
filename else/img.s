;做一个img镜像

;load是位于头几个扇区加载器
incbin "load/load"		;[0x0,0xffff]=[0,127]
times 0x10000-($-$$)db 0

;这俩加起来是裸核
incbin "cpu/cpu"		;[0x10000,0x11fff]=[128,143]
times 0x12000-($-$$)db 0
incbin "my/my"			;[0x12000,0xfffff]=[144,2047]
times 0x100000-($-$$)db 0
