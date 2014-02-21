startofbinary:

mov rdx,0x10000
call rdx

paddingofbinary:
times 0x400-(paddingofbinary-startofbinary) db 0

endofbinary:
