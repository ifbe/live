;裸核，可以被grub2，bootmgr等加载器直接扔进内存然后运行

;第一部分占据[0,0x1fff]
incbin "cpu/cpu"
times 0x2000-($-$$)db 0

;第二部分占据[0x2000,0xffff]
incbin "kernel/my/my"
times 0x10000-($-$$)db 0
