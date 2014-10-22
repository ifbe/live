%define usbhome 0x150000
;清空/usb
    mov ecx,0x10000
    mov edi,usbhome
    xor rax,rax
    rep stosq
