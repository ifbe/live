;ahci，xhci，usb,fs驱动
    ;initahci
    call endofjarvis                ;ahci@0x4000
    ;initxhci
    call endofjarvis+0x2000         ;xhci@0x6000
    ;initusb
    call endofjarvis+0x4000         ;usb@0x8000
    ;initdisk
    call endofjarvis+0x8000         ;disk@0xc000

    xor rax,rax
    jmp decide
