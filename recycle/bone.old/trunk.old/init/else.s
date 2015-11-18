;ahci，xhci，usb,fs驱动
    ;initahci
    call endofjarvis                ;ahci@0x4000

;准备进入正式程序
    lea rax,[rel function4]
    mov [rel screenwhat],rax
    lea rax,[rel menu]
    mov [rel mouseormenu],rax
    lea rax,[rel dumpanscii]
    mov [rel hexoranscii],rax

    xor rax,rax
    jmp decide
