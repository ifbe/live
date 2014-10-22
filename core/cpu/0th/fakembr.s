BITS 16

startofmbr:
jmp endofmbr

paddingofmbr:
times 0x200-(paddingofmbr-startofmbr) db 0

endofmbr:
