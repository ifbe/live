;做一个img镜像

;img专用加载器
incbin "loader/load"		;[0x0,0xffff]=[0,127]
times 0x10000-($-$$)db 0

;核心
incbin "core/init"		;[0x10000,0x11fff]=[128,143]
times 0x100000-($-$$)db 0
