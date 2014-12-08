incbin "jarvis/j.bin"
times 0x2000-($-$$)db 0

incbin "ahci/ahci.bin"
times 0x4000-($-$$)db 0

incbin "xhci/xhci.bin"
times 0x6000-($-$$)db 0

incbin "usb/usb.bin"
times 0xa000-($-$$)db 0

incbin "disk/disk.bin"
times 0xe000-($-$$)db 0
