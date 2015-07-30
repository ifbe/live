;ahci，xhci，usb,fs驱动
    ;initahci
    call endofjarvis                ;ahci@0x4000

;准备进入正式程序
    mov qword [rel addr],0x100000         ;r14 memory pointer
    mov qword [rel offset],0x420         ;r15 offset pointer

    lea rax,[rel function4]
    mov [rel screenwhat],rax
    lea rax,[rel menu]
    mov [rel mouseormenu],rax
    lea rax,[rel dumpanscii]
    mov [rel hexoranscii],rax

    xor rax,rax
    jmp decide
