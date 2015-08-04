%define usbhome 0x50000
;清空/usb
    mov ecx,0x10000
    mov edi,usbhome
    xor rax,rax
    rep stosb
