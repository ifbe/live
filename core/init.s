;裸核，可以被grub2，bootmgr等加载器直接扔进内存然后运行

;第一部分占据[0,0x1fff]
;core/cpu里面有几种可选，分别是初始化到平坦64位，保护模式，实模式，诡异模式
incbin "cpu/flat64/cpu.bin"
times 0x2000-($-$$)db 0

;第二部分占据[0x2000,0xffff]
;core/kernel是cpu初始化之后干的事，my是我的，搞定了ahci，xhci，文件系统等驱动
incbin "kernel/my/my.bin"
times 0x10000-($-$$)db 0
