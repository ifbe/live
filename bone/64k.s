;生成的init.bin可以被grub2，bootmgr等加载器直接扔进内存然后裸奔

;[0,0x1fff]
incbin "skull.bin"
times 0x2000-($-$$)db 0

;[0x2000,0x3fff]
incbin "trunk.bin"
times 0x4000-($-$$)db 0

incbin "extremities.bin"
times 0x10000-($-$$)db 0
