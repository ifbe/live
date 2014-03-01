startofbinary:

mov rdx,0xc000
call rdx

mov rdx,0xc800
call rdx

mov rdx,0x10000
call rdx

sleep:hlt
jmp sleep

paddingofbinary:
times 0x800-(paddingofbinary-startofbinary) db 0

endofbinary:
