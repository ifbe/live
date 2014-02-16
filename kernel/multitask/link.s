;[0x0,0xffff]=[0,127]
incbin "../out/load"
times 65536-($-$$)db 0

;[0x10000,0x1ffff]=[128,255]
incbin "../out/live.bin"
times 131072-($-$$)db 0

;[0x20000,0x27fff]=[256,319]
incbin "../out/ball.bin"
times 163840-($-$$)db 0

;[0x28000,0x2ffff]=[320,383]
incbin "../out/taiji.bin"
times 196608-($-$$)db 0

;[0x30000,0x7ffff]=[384,1023]
incbin "../else/jarvis.jpg"
times 524288-($-$$)db 0
