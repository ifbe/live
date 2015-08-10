[bits 64]
startofwrap:


jmp endofwrap


paddingofwrap:
times 0x1000-(paddingofwrap-startofwrap) db 0


endofwrap: