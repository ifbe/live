;生成的init.bin可以被grub2，bootmgr等加载器直接扔进内存然后裸奔

;[0,0x1fff]
incbin "cpu/cpu.bin"
times 0x2000-($-$$)db 0

;[0x2000,0xffff]
incbin "jarvis/j.bin"
times 0x4000-($-$$)db 0

incbin "ahci/ahci.bin"
times 0x6000-($-$$)db 0

incbin "xhci/xhci.bin"
times 0x8000-($-$$)db 0

incbin "usb/usb.bin"
times 0xc000-($-$$)db 0

incbin "disk/disk.bin"
times 0x10000-($-$$)db 0
