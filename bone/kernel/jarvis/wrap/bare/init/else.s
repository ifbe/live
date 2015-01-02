;ahci，xhci，usb,fs驱动
    ;initahci
    call endofjarvis                ;ahci@0x4000
    ;initxhci
    call endofjarvis+0x2000         ;xhci@0x6000
    ;initusb
    call endofjarvis+0x4000         ;usb@0x8000
    ;initdisk
    call endofjarvis+0x8000         ;disk@0xc000

;准备进入正式程序
    lea rax,[rel function4]
    mov [rel screenwhat],rax
    lea rax,[rel menu]
    mov [rel mouseormenu],rax
    lea rax,[rel dumpanscii]
    mov [rel hexoranscii],rax

    xor rax,rax
    jmp decide
