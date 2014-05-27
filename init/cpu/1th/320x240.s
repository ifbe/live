BITS 16
startofscreen:

    mov ax,0x13
    int 0x10
    jmp endofscreen


paddingofscreen:
times 0x200-(paddingofscreen-startofscreen) db 0


endofscreen:
