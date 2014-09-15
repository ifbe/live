BITS 16
startofscreen:

    jmp endofscreen


paddingofscreen:
times 0x100-(paddingofscreen-startofscreen) db 0

endofscreen:
