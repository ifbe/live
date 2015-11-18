BITS 16
startofscreen:

    jmp endofscreen


paddingofscreen:
times 0x400-(paddingofscreen-startofscreen) db 0

endofscreen:
