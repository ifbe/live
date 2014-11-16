;生成的init.bin可以被grub2，bootmgr等加载器直接扔进内存然后裸奔

;[0,0x1fff]
incbin "core.bin"
times 0x4000000-($-$$)db 0
